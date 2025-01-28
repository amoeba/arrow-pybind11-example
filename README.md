# arrow-pybind11-exmaple

Minimal example of how to pass a PyArrow Table to a C++ extension in a way that's stable and safer than passing and working with the Table directly.

Uses the Arrow C Stream Interface (`ArrowArrayStream`) and is (1) ABI-stable and (2) minimal in terms of copies.

## Pre-requisites

- Python 3
- CMake
- libarrow (Arrow C++)
- A standard C++ compiler toolchain (gcc, clang)

## Setup

```sh
python -m venv .venv
./.venv/bin/activate
pip install .
python example.py
```

## Running

Run `./test.py`, which looks like this:

```python
import example
import pyarrow as pa

tbl = pa.Table.from_pydict({"x": [1, 2, 3, 4, 5]})
reader = pa.RecordBatchReader.from_batches(tbl.schema, tbl.to_batches())

example.work_with_table(reader)
```

Which, when run, prints:

```sh
Got Table:
x: int64
----
x:
  [
    [
      1,
      2,
      3,
      4,
      5
    ]
  ]

Got column `x` which has 1 chunks and 5 rows.
```

## Development

I like to have [clangd](https://clangd.llvm.org) while I work and this is what's needed for that:

```sh
pip install --no-cache-dir -e . -v --config-settings=build-dir=build --no-build-isolation
```

## Related

- See Weston Pace's example working with Arrow Arrays: <https://github.com/westonpace/arrow-cdata-example>
