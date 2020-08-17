#!/bin/env python3
import argparse
import os

ROO_DIR = os.path.dirname(os.path.abspath(__file__))

parser = argparse.ArgumentParser(description='Process some integers.')
parser.add_argument('--upload_flags', type=str, action='append')
parser.add_argument('--upload_port', type=str)
parser.add_argument('--monitor_port', type=str)


def getTemplate():
    with open(os.path.abspath(os.path.join(ROO_DIR, './platformio.ini'))) as file:
        return file.read()


def writeConfigFile(data: str):
    with open(os.path.abspath(os.path.join(ROO_DIR, '../platformio.ini')), 'w') as file:
        return file.write(data)


def render_template(template: str, args: dict):
    rendered_template = template
    for arg in args.keys():
        key = f"{{{{ {arg.upper()} }}}}"
        v = args.get(arg)
        value = v if isinstance(v, str) else '\t'+"\n\t".join(v)
        rendered_template = rendered_template.replace(key, value)
    return rendered_template


def main():
    args = parser.parse_args()
    template = getTemplate()
    data = render_template(
        template,
        {
            "upload_flags": args.upload_flags,
            "upload_port": args.upload_port,
            "monitor_port": args.monitor_port,
        }
    )
    writeConfigFile(data)


if __name__ == "__main__":
    main()
