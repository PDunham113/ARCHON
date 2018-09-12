"""Command-line and functional interface for writing RGB strings."""
from enum import auto, Enum

import Comm

COLOR_ESCAPE_CHAR = "#"

# Predefined single-character color codes
COLOR_MAP = {
    "R": bytes(0xFF, 0x00, 0x00),
    "O": bytes(0xFF, 0x7F, 0x00),
    "Y": bytes(0xFF, 0xFF, 0x00),
    "G": bytes(0x00, 0xFF, 0x00),
    "B": bytes(0x00, 0x00, 0xFF),
    "I": bytes(0x4B, 0x00, 0x82),
    "V": bytes(0x94, 0x00, 0xD3),
    "W": bytes(0xFF, 0xFF, 0xFF),
    "-": bytes(0x00, 0x00, 0x00),
}

LED_DATA_MSG = bytes(0x04)
MAX_LED_IN_MSG = 4


def add_color_code(code: str, value: bytes, update: bool = False):
    """Add single-character color code to COLOR_MAP for session.

    Values currently in COLOR_MAP cannot be redefined, unless the `update` flag
    is set. The value in COLOR_ESCAPE_CHAR is reserved, and cannot be
    redefined.

    @param code Single character to use as color code.
    @param value 3-digit bytes object containing 24-bit RGB color value.
    @param update Enables overwriting of color codes. Disabled by default.

    @returns None.

    @raises ValueError When color code is reserved and update flag is not set.
    """
    # Kill if key exists
    if (code in COLOR_MAP.keys() and not update) or code == COLOR_ESCAPE_CHAR:
        raise ValueError("Key {} already reserved.".format(code))
    else:
        COLOR_MAP[code] = value


def generate_message(colors: str, led_pos: int = 0) -> tuple[Comm.CommMessage]:
    """Generate iterable with corresponding message packets from color string.

    Messages are parsed one character at a time. Characters are assumed to be
    predefined color codes, unless escaped with the COLOR_ESCAPE_CHAR. Escaped
    sequences are expected to be 6-character RGB hex strings.

    It is assumed that all LEDs are sequential.

    @param colors String containing color codes and escaped color values.
    @param led_pos Starting LED position of color string.

    @returns Tuple containing CommMessage packets.

    @raises KeyError When color code is undefined.
    @raises ValueError When escaped sequence is not a valid hex string.
    """
    class ParseState(Enum):
        """Define state of color string parser."""
        NO_LED = auto()
        HEX_PARSE = auto()
        LED_COMPLETE = auto()

    msg_stream = []
    color_buffer = []
    parse_state = ParseState.NO_LED

    # Parse string one character at a time
    for char in colors:
        # Calculate LED bytes
        if parse_state is ParseState.NO_LED:
            if char == COLOR_ESCAPE_CHAR:
                # Start hex sequence
                color_buffer = []
                parse_state = ParseState.HEX_PARSE
            else:
                next_color = COLOR_MAP[char]
                parse_state = ParseState.LED_COMPLETE
        elif parse_state is ParseState.HEX_PARSE:
            # Add next character
            color_buffer.append(char)
            # If we have 6, convert to bytes
            if len(color_buffer) == 6:
                next_color = bytes.fromhex(color_buffer)
                parse_state = ParseState.LED_COMPLETE
        # If we've finished an LED, add it to the list
        if parse_state is ParseState.LED_COMPLETE:
            msg_stream.append(bytes((led_pos,)) + next_color)
            led_pos += 1
            parse_state = ParseState.NO_LED
    # Once done, create iterable of CommMessage tuples
    msg_stream = (
        Comm.calc_message(
            LED_DATA_MSG, b"".join(msg_stream[i, i + MAX_LED_IN_MSG])
        ) for i in range(0, len(msg_stream), MAX_LED_IN_MSG)
    )

    return msg_stream
