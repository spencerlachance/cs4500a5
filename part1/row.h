//lang::CwC

#pragma once

#include "object.h"
#include "array.h"
#include "schema.h"
#include "visitors.h"

/*************************************************************************
 * Row::
 *
 * This class represents a single row of data constructed according to a
 * dataframe's schema. The purpose of this class is to make it easier to add
 * read/write complete rows. Internally a dataframe hold data in columns.
 * Rows have pointer equality.
 * 
 * @author Spencer LaChance <lachance.s@husky.neu.edu>
 * @author David Mberingabo <mberingabo.d@husky.neu.edu>
 */
class Row : public Object {
    public:
        IntArray* col_types_;
        Array* fields_;
        size_t idx_;
 
        /** Build a row following a schema. */
        Row(Schema& scm) {
            col_types_ = new IntArray();
            col_types_->append_all(scm.get_types());
            fields_ = new Array();
            idx_ = -1;

            for (int i = 0; i < col_types_->size(); i++) {
                char type = col_types_->get(i);
                switch (type) {
                    case 'I':
                        fields_->append(new IntArray());
                        break;
                    case 'B':
                        fields_->append(new BoolArray());
                        break;
                    case 'F':
                        fields_->append(new FloatArray());
                        break;
                    case 'S':
                        fields_->append(new Array());
                        break;
                    default:
                        exit_if_not(false, "Invalid type found.");
                }
            }
        }

        /** Destructor */
        ~Row() {
            delete col_types_;
            delete fields_;
        }
        
        /** Setters: set the given column with the given value. Setting a column with
            * a value of the wrong type is undefined. */
        void set(size_t col, int val) {
            exit_if_not(col < width(), "Column index out of bounds.");
            IntArray* arr = dynamic_cast<IntArray*>(fields_->get(col));
            if (arr == nullptr) {
                exit_if_not(false, "Column index corresponds to the wrong type.");
            }
            arr->set(val, 0);
        }
        void set(size_t col, float val) {
            exit_if_not(col < width(), "Column index out of bounds.");
            FloatArray* arr = dynamic_cast<FloatArray*>(fields_->get(col));
            if (arr == nullptr) {
                exit_if_not(false, "Column index corresponds to the wrong type.");
            }
            arr->set(val, 0);
        }
        void set(size_t col, bool val) {
            exit_if_not(col < width(), "Column index out of bounds.");
            BoolArray* arr = dynamic_cast<BoolArray*>(fields_->get(col));
            if (arr == nullptr) {
                exit_if_not(false, "Column index corresponds to the wrong type.");
            }
            arr->set(val, 0);
        }
        /** Acquire ownership of the string. */
        void set(size_t col, String* val) {
            exit_if_not(col < width(), "Column index out of bounds.");
            Array* arr = dynamic_cast<Array*>(fields_->get(col));
            if (arr == nullptr) {
                exit_if_not(false, "Column index corresponds to the wrong type.");
            }
            arr->set(val, 0);
        }
        
        /** Set/get the index of this row (ie. its position in the dataframe. This is
         *  only used for informational purposes, unused otherwise */
        void set_idx(size_t idx) {
            idx_ = idx;
        }
        size_t get_idx() {
            return idx_;
        }
        
        /** Getters: get the value at the given column. If the column is not
            * of the requested type, the result is undefined. */
        int get_int(size_t col) {
            exit_if_not(col < width(), "Column index out of bounds.");
            IntArray* arr = dynamic_cast<IntArray*>(fields_->get(col));
            if (arr == nullptr) {
                exit_if_not(false, "Column index corresponds to the wrong type.");
            }
            arr->get(0);
        }
        bool get_bool(size_t col) {
            exit_if_not(col < width(), "Column index out of bounds.");
            BoolArray* arr = dynamic_cast<BoolArray*>(fields_->get(col));
            if (arr == nullptr) {
                exit_if_not(false, "Column index corresponds to the wrong type.");
            }
            arr->get(0);
        }
        float get_float(size_t col) {
            exit_if_not(col < width(), "Column index out of bounds.");
            FloatArray* arr = dynamic_cast<FloatArray*>(fields_->get(col));
            if (arr == nullptr) {
                exit_if_not(false, "Column index corresponds to the wrong type.");
            }
            arr->get(0);
        }
        String* get_string(size_t col) {
            exit_if_not(col < width(), "Column index out of bounds.");
            Array* arr = dynamic_cast<Array*>(fields_->get(col));
            if (arr == nullptr) {
                exit_if_not(false, "Column index corresponds to the wrong type.");
            }
            return dynamic_cast<String*>(arr->get(0));
        }
        
        /** Number of fields in the row. */
        size_t width() {
            return col_types_->size();
        }
        
        /** Type of the field at the given position. An idx >= width is  undefined. */
        char col_type(size_t idx) {
            exit_if_not(idx < width(), "Column type index out of bounds.");
            return col_types_->get(idx);
        }
        
        /** Given a Fielder, visit every field of this row. The first argument is
            * index of the row in the dataframe.
            * Calling this method before the row's fields have been set is undefined. */
        void visit(size_t idx, Fielder& f) {
            f.start(idx);
            for (int i = 0; i < col_types_->size(); i++) {
                char type = col_types_->get(i);
                switch (type) {
                    case 'I':
                        f.accept(get_int(i));
                        break;
                    case 'B':
                        f.accept(get_bool(i));
                        break;
                    case 'F':
                        f.accept(get_float(i));
                        break;
                    case 'S':
                        f.accept(get_string(i));
                        break;
                    default:
                        exit_if_not(false, "Invalid type found.");
                }
            }
            f.done();
        }

        /** Getter for this row's schema's types. */
        IntArray* get_types() {
            return col_types_;
        }
};