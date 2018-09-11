"""Python-side communication with COMM.c.

Commands:
    COM_PKT_EMPTY <0x00>
        Completely empty packet, 0 length, no data. Use for heartbeats, etc.
    COM_PKT_TEST <0x01>
        No restrictions on contents, use for testing only. Each packet may
        contain an arbitrary number of LED data entries.
    COM_PKT_BUSY
        Indicates the device sending is busy - data is ignored. Packets sent to
        the device are not guaranteed to be processed until device sends a
        COM_PKT_READY.
    COM_PKT_LED_CTRL
        Execute one of various LED commands, specified by the first data byte.
        Supported commands are:

        CHANGE_BLOCK <0x42>
            Change which 8-bit block the LED_DATA packet writes to. Second byte
            specifies the new block to write to.
        COPY <0x43>
            Copy the value of one LED to multiple other LEDs. Second byte
            specifies the parent LED, all other bytes specify child LEDs.
        ERASE <0x45>
            Sets all LEDs within the block to 0. No additional parameters.
        PUSH <0x50>
            Force an immediate update of the RGB LEDs with whatever is in the
            buffers. No additional parameters.
    COM_PKT_LED_DATA
        Update with new RGB LED data, each LED is expressed in 4 bytes. The
        format is:
        ```
        <LED_NUM><R_VAL><G_VAL><B_VAL>
        ```
    COM_PKT_READY
        Indicates the device sending is ready to recieve - data is ignored.
        Packets sent to the device are guaranteed to be processed until
        device sends a COM_PKT_BUSY.

NOTE: The byte values of com_type are currently left undefined, except for
      COM_PKT_EMPTY and COM_PKT_TEST.
"""
import argparse
from collections import namedtuple

import serial

DEFAULT_PORT = "/dev/ttyUSB0/"
DEFAULT_BAUD = 9600

START_CHAR = b"\x15"

CommMessage = namedtuple("CommMessage", [
    "type",  # type: bytearray
    "length",  # type: bytearray
    "data",  # type: bytearray
    "checksum",  # type: bytearray
])


def calc_checksum(msg: CommMessage) -> CommMessage:
    """Calculate message checksum from individual components.

    Currently XORS all of the things together. Checksum is calculated in-place.
    It is expected that the checksum bytearray has one element initialized to
    0.

    @param CommMessage Message to calculate checksum for.

    @returns CommMessage tuple with calculated checksum.

    @raises None.
    """
    checksum = msg.length[0] ^ msg.type[0]

    for byte in msg.data:
        checksum ^= byte

    msg.checksum[0] = checksum

    return msg


def calc_message(type: bytearray, data: bytearray) -> CommMessage:
    """Calculate length & checksum of message given data & type.

    @param type Number representing type of message.
    @param data Iterable containing message data.

    @returns CommMessage tuple containing calculated message.

    @raises None.
    """
    message = CommMessage(type, bytearray([len(data)]), data, bytearray(1))
    calc_checksum(message)

    return message


def send_message(port: serial.Serial, msg: CommMessage):
    """Send the calculated message over serial.

    Sends bytes in the following order:
    -   SOM character
    -   Message type
    -   Message length
    -   Message data
    -   Message checksum

    Will only send up to msg.length bytes - no more, no less.

    @param port The serial port to send the message over - it is assumed to be
                open.
    @param msg CommMessage tuple containing calculated message.

    @returns None.

    @raises None.
    """
    port.write(START_CHAR)
    port.write(msg.type)
    port.write(msg.length)
    port.write(msg.data[:msg.length])
    port.write(msg.checksum)


def print_message(msg: CommMessage):
    """Print CommMessage in terminal.

    Prints hexadecimal representation of each byte in message, in addition to
    the field to which each byte belongs.

    @param msg CommMessage tuple containing message to print.

    @returns None.

    @raises None.
    """
    text_msg = [
        "SOM:\t0x{}\n".format(START_CHAR.hex()),
        "TYPE:\t0x{}\n".format(msg.type.hex()),
        "LENGTH:\t0x{}\n".format(msg.length.hex()),
        "DATA:\t{}\n".format(":".join(
            ["0x{}".format(msg.data.hex()[i:i + 2])
             for i in range(0, len(msg.data.hex()), 2)])),
        "CHKSM:\t0x{}\n".format(msg.checksum.hex()),
    ]

    print("".join(text_msg))


if __name__ == "__main__":
    """Interface for testing and sending messages to RGB hardware."""
    parser = argparse.ArgumentParser(description="RGB Communication Terminal")

    parser.add_argument("command_type", type=int)

    parser.add_argument("command_data", type=int, nargs="+")

    args = parser.parse_args()

    # Convert to bytearray
    args.command_type = bytearray([args.command_type])
    args.command_data = bytearray(args.command_data)

    msg = calc_message(args.command_type, args.command_data)

    print_message(msg)
