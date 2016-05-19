# coding: utf-8
import json
import argparse

from json_scell import SCellJSONEncoderDecoder, ReverseJSONValue
__author__ = 'Lagovas <lagovas.lagovas@gmail.com>'


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("shared_key")
    parser.add_argument("input")
    parser.add_argument("output")
    parser.add_argument("-d", "--decrypt", help="Decrypt", action="store_true")
    parser.add_argument("-e", "--encrypt", help="Encrypt", action="store_true")

    parser.add_argument("-t", "--themis", help="Encrypt using themis", action="store_true")
    parser.add_argument("-r", "--reverse", help="Reverse values", action="store_true")

    args = parser.parse_args()

    if args.decrypt and args.encrypt:
        print("You can pass only one argument: -e or -d")
        exit(1)

    if not (args.decrypt or args.encrypt):
        print("Choose one of the modes: -e or -d")
        exit(1)

    with open(args.input, 'r') as f:
        input_json = json.loads(f.read())


    coder = ReverseJSONValue() if args.reverse else SCellJSONEncoderDecoder(args.shared_key)
    if args.encrypt:
        output_data = coder.encode(input_json)
    else:
        output_data = coder.decode(input_json)
    with open(args.output, 'w') as f:
        f.write(json.dumps(output_data))
