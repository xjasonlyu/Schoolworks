# XFTP (12X File Transport Protocol)

A UDP-based, buggy file transport C/S written in Python.

## Protocol

```Python
# General Header
# |    1 byte    | 1 byte | 1 byte | 8 byte | n byte |
# | MAGIC_NUMBER | STATUS |  CMD   |   ID   |  DATA  |

# LIST REQUEST EXAMPLE:
# +------+---------+------+----------------------------------+-------+
# |  MN  | REQUEST | CMD  |           RANDOM ID              |  DIR  |
# +------+---------+------+----------------------------------+-------+
# | \xcf |  \x00   | \x00 | \x0b\x1c\xd9\xb3\x1e\x98\x50\x1f | /root |
# +------+---------+------+----------------------------------+-------+
#
# LIST REPLY EXAMPLE:
# +------+----------+------+----------------------------------+------------------------+
# |  MN  | REPLY_OK | CMD  |           RANDOM ID              |      DATA in JSON      |
# +------+----------+------+----------------------------------+------------------------+
# | \xcf |   \x01   | \x00 | \x0b\x1c\xd9\xb3\x1e\x98\x50\x1f | {"files":[],"dirs",[]} |
# +------+----------+------+----------------------------------+------------------------+

# UPLOAD/DOWNLOAD REQUEST EXAMPLE:
# +------+---------+--------------+----------------------------------+-------------+
# |  MN  | REQUEST |     CMD      |           RANDOM ID              |  FILE NAME  |
# +------+---------+--------------+----------------------------------+-------------+
# | \xcf |  \x00   | \x01 or \x02 | \x0b\x1c\xd9\xb3\x1e\x98\x50\x1f |  /tmp/1.txt |
# +------+---------+--------------+----------------------------------+-------------+
#
# UPLOAD/DOWNLOAD REPLY EXAMPLE:
# +------+----------+--------------+----------------------------------+-------------------+
# |  MN  | REPLY_OK |     CMD      |           RANDOM ID              | PORT (e.g. 59830) |
# +------+----------+--------------+----------------------------------+-------------------+
# | \xcf |   \x01   | \x01 or \x02 | \x0b\x1c\xd9\xb3\x1e\x98\x50\x1f |     \xb6\xe9      |
# +------+----------+--------------+----------------------------------+-------------------+


```

## Constant

```Python
# XFTP Version Byte
MAGIC_NUMBER = 0xCF

# CMD
LIST     = 0x00
UPLOAD   = 0x01
DOWNLOAD = 0x02
DELETE   = 0x03
MOVE     = 0x04
MKDIR    = 0x05
NOTHING  = 0xFF

# STATUS
REQUEST              = 0x00
REPLY_OK             = 0x01
REPLY_INVALID        = 0x02
REPLY_NOT_FOUND      = 0x03
REPLY_FILE_EXISTS    = 0x04
REPLY_INTERNAL_ERROR = 0x05
REPLY_ACK            = 0x06
```
