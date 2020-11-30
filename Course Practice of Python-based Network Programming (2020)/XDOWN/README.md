# XDOWN

## Protocol

```python
# Client Request
# +-----+-------+----------+
# | VER | NFILE | FILENAME |
# +-----+-------+----------+
# |  1  |   1   |     N    |
# +-----+-------+----------+
#
# Server Reply
# +-----+-----+---------+
# | VER | REP | FILELEN |
# +-----+-----+---------+
# |  1  |  1  |    8    |
# +-----+-----+---------+
```

## Constants

```python
VERSION = 0x01

REP_SUCCESS = 0x00
REP_NOT_FOUND = 0x01
REP_INTERNAL_ERROR = 0x02
REP_INVALID_REQUEST = 0x03
```
