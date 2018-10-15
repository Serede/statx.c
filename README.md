# statx.c
C wrapper for statx - get file status (extended)

## Usage
```
statx [OPTION...] file
C wrapper for statx - get file status (extended)

  -a, --atime                Display file access time only.
                             Format: sec.nsec (Epoch)
  -b, --btime                Display file birth time only.
                             Format: sec.nsec (Epoch)
  -c, --ctime                Display file change time only.
                             Format: sec.nsec (Epoch)
  -m, --mtime                Display file modification time only.
                             Format: sec.nsec (Epoch)
  -?, --help                 Give this help list
      --usage                Give a short usage message
```