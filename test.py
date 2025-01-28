import example
import pyarrow as pa

tbl = pa.Table.from_pydict({"x": [1, 2, 3, 4, 5]})
reader = pa.RecordBatchReader.from_batches(tbl.schema, tbl.to_batches())

example.work_with_table(reader)
