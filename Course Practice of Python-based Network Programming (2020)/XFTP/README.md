# XFTP (12X File Transport Protocol)

A UDP-based, buggy file transport C/S written in Python.

## Protocol

```shell
# |    1 byte    | 1 byte | 1 byte | 8 byte |     1 byte       |  n byte   |
# | MAGIC_NUMBER | STATUS |  CMD   |   ID   | FILE NAME LENGTH | FILE NAME |
```
