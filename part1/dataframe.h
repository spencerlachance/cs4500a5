//lang::CwC + a little Cpp

#pragma once

#include "array.h"
#include "helper.h"
#include "schema.h"
#include "column.h"
#include "row.h"

/**
 * Fielder that prints each field.
 * 
 * @author David Mberingabo <mberingabo.d@husky.neu.edu>
 * @author Spencer LaChance <lachance.s@husky.neu.edu>
 */
class PrintFielder : public Fielder {
    public:
        void start(size_t r) { }
        void accept(bool b) { printf("<%d>", b); }
        void accept(float f) { printf("<%f>", f); }
        void accept(int i) { printf("<%d>", i); }
        void accept(String* s) { printf("<%s>", s->c_str()); }
        void done() { }
};

/**
 * Rower that prints each row.
 * 
 * @author David Mberingabo <mberingabo.d@husky.neu.edu>
 * @author Spencer LaChance <lachance.s@husky.neu.edu>
 */
class PrintRower : public Rower {
    public:
        PrintFielder* pf_;

        PrintRower() {
            pf_ = new PrintFielder();
        }

        ~PrintRower() {
            delete pf_;
        }

        bool accept(Row& r) { 
            r.visit(r.get_idx(), *pf_);
            printf("\n");
        }

        void join_delete(Rower* other) { }
};
 
/****************************************************************************
 * DataFrame::
 *
 * A DataFrame is table composed of columns of equal length. Each column
 * holds values of the same type (I, S, B, F). A dataframe has a schema that
 * describes it.
 * 
 * @author Spencer LaChance <lachance.s@husky.neu.edu>
 * @author David Mberingabo <mberingabo.d@husky.neu.edu>
 */
class DataFrame : public Object {
    public:
        Array* columns_;
        Schema* schema_;
        // Number of rows
        size_t length_;
        
        // Used for pmap():
        // The original Rower passed to pmap() that runs on the first quarter of the df.
        Rower* r_;
        // The clone Rowers that run on the other quarters of the df.
        Rower* r2_;
 
        /** Create a data frame with the same columns as the given df but with no rows or rownames */
        DataFrame(DataFrame& df) {
            columns_ = new Array();
            columns_->append_all(df.get_columns());
            schema_ = new Schema(df.get_schema());
            schema_->clear_row_names();
            length_ = df.nrows();
        }
        
        /** Create a data frame from a schema and columns. All columns are created
          * empty. */
        DataFrame(Schema& schema) {
            IntArray* types = schema.get_types();
            columns_ = new Array();
            schema_ = new Schema(schema);
            length_ = 0;
        }

        /** Destructor */
        ~DataFrame() {
            delete columns_;
            delete schema_;
        }
        
        /** Returns the dataframe's schema. Modifying the schema after a dataframe
          * has been created in undefined. */
        Schema& get_schema() {
            return *schema_;
        }
        
        /** Adds a column this dataframe, updates the schema, the new column
          * is external, and appears as the last column of the dataframe, the
          * name is optional and external. A nullptr column is undefined. */
        void add_column(Column* col, String* name) {
            exit_if_not(col != nullptr, "Undefined column provided.");
            if (col->size() < length_) {
                pad_column(col);
            } else if (col->size() > length_) {
                length_ = col->size();
                for (int i = 0; i < columns_->size(); i++) {
                    pad_column(dynamic_cast<Column*>(columns_->get(i)));
                }
            }
            columns_->append(col);
            if (columns_->size() > schema_->width()) {
                schema_->add_column(col->get_type(), name);
            }
        }
        
        /** Return the value at the given column and row. Accessing rows or
         *  columns out of bounds, or request the wrong type is undefined.*/
        int get_int(size_t col, size_t row) {
            IntColumn* column = dynamic_cast<Column*>(columns_->get(col))->as_int();
            exit_if_not(column != nullptr, "Column index corresponds to the wrong type.");
            return column->get(row);
        }
        bool get_bool(size_t col, size_t row) {
            BoolColumn* column = dynamic_cast<Column*>(columns_->get(col))->as_bool();
            exit_if_not(column != nullptr, "Column index corresponds to the wrong type.");
            return column->get(row);
        }
        float get_float(size_t col, size_t row) {
            FloatColumn* column = dynamic_cast<Column*>(columns_->get(col))->as_float();
            exit_if_not(column != nullptr, "Column index corresponds to the wrong type.");
            return column->get(row);
        }
        String* get_string(size_t col, size_t row) {
            StringColumn* column = dynamic_cast<Column*>(columns_->get(col))->as_string();
            exit_if_not(column != nullptr, "Column index corresponds to the wrong type.");
            return column->get(row);
        }
        
        /** Return the offset of the given column name or -1 if no such col. */
        int get_col(String& col) {
            return schema_->col_idx(col.c_str());
        }
        
        /** Return the offset of the given row name or -1 if no such row. */
        int get_row(String& row) {
            return schema_->row_idx(row.c_str());
        }
        
        /** Set the value at the given column and row to the given value.
          * If the column is not  of the right type or the indices are out of
          * bound, the result is undefined. */
        void set(size_t col, size_t row, int val) {
            IntColumn* column = dynamic_cast<Column*>(columns_->get(col))->as_int();
            exit_if_not(column != nullptr, "Column index corresponds to the wrong type.");
            column->set(row, val);
        }
        void set(size_t col, size_t row, bool val) {
            BoolColumn* column = dynamic_cast<Column*>(columns_->get(col))->as_bool();
            exit_if_not(column != nullptr, "Column index corresponds to the wrong type.");
            column->set(row, val);
        }
        void set(size_t col, size_t row, float val) {
            FloatColumn* column = dynamic_cast<Column*>(columns_->get(col))->as_float();
            exit_if_not(column != nullptr, "Column index corresponds to the wrong type.");
            column->set(row, val);
        }
        void set(size_t col, size_t row, String* val) {
            StringColumn* column = dynamic_cast<Column*>(columns_->get(col))->as_string();
            exit_if_not(column != nullptr, "Column index corresponds to the wrong type.");
            column->set(row, val);
        }
        
        /** Set the fields of the given row object with values from the columns at
          * the given offset.  If the row is not form the same schema as the
          * dataframe, results are undefined. */
        void fill_row(size_t idx, Row& row) {
            exit_if_not(schema_->get_types()->equals(row.get_types()), "Row's schema does not match the data frame's.");
            for (int j = 0; j < ncols(); j++) {
                Column* col = dynamic_cast<Column*>(columns_->get(j));
                char type = col->get_type();
                switch (type) {
                    case 'I':
                        row.set(j, col->as_int()->get(idx));
                        break;
                    case 'B':
                        row.set(j, col->as_bool()->get(idx));
                        break;
                    case 'F':
                        row.set(j, col->as_float()->get(idx));
                        break;
                    case 'S':
                        row.set(j, col->as_string()->get(idx));
                        break;
                    default:
                        exit_if_not(false, "Column has invalid type.");
                }
            }
        }
        
        /** Add a row at the end of this dataframe. The row is expected to have
          * the right schema and be filled with values, otherwise undefined.  */
        void add_row(Row& row) {
            exit_if_not(schema_->get_types()->equals(row.get_types()), "Row's schema does not match the data frame's.");
            for (int j = 0; j < ncols(); j++) {
                Column* col = dynamic_cast<Column*>(columns_->get(j));
                char type = col->get_type();
                switch (type) {
                    case 'I':
                        col->push_back(row.get_int(j));
                        break;
                    case 'B':
                        col->push_back(row.get_bool(j));
                        break;
                    case 'F':
                        col->push_back(row.get_float(j));
                        break;
                    case 'S':
                        col->push_back(row.get_string(j));
                        break;
                    default:
                        exit_if_not(false, "Column has invalid type.");
                }
            }
            length_++;
        }
        
        /** The number of rows in the dataframe. */
        size_t nrows() {
            return length_;
        }
        
        /** The number of columns in the dataframe.*/
        size_t ncols() {
            return columns_->size();
        }
        
        /** Visit rows in order */
        void map(Rower& r) {
            Row* row = new Row(*schema_);
            for (int i = 0; i < length_; i++) {
                row->set_idx(i);
                fill_row(i, *row);
                r.accept(*row);
            }
            delete row;
        }
        
        /** Create a new dataframe, constructed from rows for which the given Rower
          * returned true from its accept method. */
        DataFrame* filter(Rower& r) {
            DataFrame* df = new DataFrame(*schema_);
            for (int i = 0; i < length_; i++) {
                Row* row = new Row(*schema_);
                fill_row(i, *row);
                if (r.accept(*row)) {
                    df->add_row(*row);
                }
                delete row;
            }
            return df;
        }
        
        /** Print the dataframe in SoR format to standard output. */
        void print() {
            PrintRower* pr = new PrintRower();
            map(*pr);
            printf("\n");
            delete pr;
        }

        /** Getter for the dataframe's columns. */
        Array* get_columns() {
            return columns_;
        }

        /** Pads the given column with a default value until its length
         *  matches the number of rows in the data frame. */
        void pad_column(Column* col) {
            char type = col->get_type();
            while (col->size() < length_) {
                switch(type) {
                    case 'I':
                        col->push_back(0);
                        break;
                    case 'B':
                        col->push_back(false);
                        break;
                    case 'F':
                        col->push_back(0);
                        break;
                    case 'S':
                        col->push_back("");
                        break;
                    default:
                        exit_if_not(false, "Invalid column type.");
                }
            }
        }
};
