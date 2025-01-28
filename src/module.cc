#include <iostream>

#include <arrow/c/bridge.h>
#include <arrow/table.h>
#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

void work_with_table(const pybind11::handle &reader) {
  if (!pybind11::hasattr(reader, "_export_to_c")) {
    throw pybind11::type_error(
        "Expected object to implement _export_to_c interface.");
  }

  /// This is a three-step process:
  /// 1. Export our RecordBatchReader as an ArrowArrayStrem
  /// 2. Import the ArrowArrayStream back and convert to a Table
  /// 3. Clean up after ourselves

  ///
  /// Part 1: Export the PyArrow RecordBatchReader over C Stream Interface
  ///

  // Create an ArrowArrayStream struct to hold the stream we export
  ArrowArrayStream c_stream;

  // We need to cast our struct to a pointer to be able to use it
  intptr_t stream_ptr = reinterpret_cast<intptr_t>(&c_stream);

  // Call the Python object's _export_to_c method to export the stream
  auto export_fn = reader.attr("_export_to_c");
  export_fn(stream_ptr);

  ///
  /// Part 2: Import it back and re-create the arrow::Table
  ///

  // Create a RecordBatchReader from the stream
  auto import_result = arrow::ImportRecordBatchReader(&c_stream);

  if (!import_result.ok()) {
    throw pybind11::value_error("Unexpected error from arrow-c++: " +
                                import_result.status().ToString());
  }
  auto record_batch_reader = import_result.ValueUnsafe();

  // Convert the RecordBatchReader into a Table
  auto to_table_result = record_batch_reader->ToTable();

  if (!to_table_result.ok()) {
    throw pybind11::value_error("Unexpected error from arrow-c++: " +
                                to_table_result.status().ToString());
  }

  auto table = to_table_result.ValueUnsafe();

  // And print it out so we can check we have the right result
  std::cout << "Got Table: " << std::endl;
  std::cout << table->ToString() << std::endl;

  std::string col_name = "x";
  auto col = table->GetColumnByName(col_name);
  std::cout << "Got column `" << col_name << "` which has " << col->num_chunks()
            << " chunks and " << col->length() << " rows." << std::endl;
  ///
  /// Part 3: Release the stream if needed
  ///
  if (c_stream.release) {
    c_stream.release(&c_stream);
  }

  return;
}

PYBIND11_MODULE(_core, m) {
  m.def("work_with_table", &work_with_table, R"pbdoc(
        Work with a PyArrow Table
    )pbdoc");
}
