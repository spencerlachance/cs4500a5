//lang::CwC

#pragma once

#include "object.h"
#include "array.h"

/*************************************************************************
 * Schema::
 * A schema is a description of the contents of a data frame, the schema
 * knows the number of columns and number of rows, the type of each column,
 * optionally columns and rows can be named by strings.
 * The valid types are represented by the chars 'S', 'B', 'I' and 'F'.
 * 
 * @author Spencer LaChance <lachance.s@husky.neu.edu>
 * @author David Mberingabo <mberingabo.d@husky.neu.edu>
 */
class Schema : public Object {
    public:
        IntArray* col_types_;
        Array* col_names_;
        Array* row_names_;
 
        /** Copying constructor */
        Schema(Schema& from) {
            col_types_ = new IntArray();
            col_names_ = new Array();
            row_names_ = new Array();
            col_types_->append_all(from.get_types());
            col_names_->append_all(from.get_col_names());
            row_names_->append_all(from.get_row_names());
        }
        
        /** Create an empty schema **/
        Schema() {
            col_types_ = new IntArray();
            col_names_ = new Array();
            row_names_ = new Array();
        }
        
        /** Create a schema from a string of types. A string that contains
         *  characters other than those identifying the four type results in
         *  undefined behavior. The argument is external, a nullptr argument is
         *  undefined. **/
        Schema(const char* types) {
            exit_if_not(types != nullptr, "Null types argument provided.");
            col_types_ = new IntArray();
            col_names_ = new Array();
            row_names_ = new Array();
            for (int i = 0; i < strlen(types); i++) {
                char c = types[i];
                exit_if_not(c == 'S' || c == 'B' || c == 'I' || c == 'F', 
                "Invalid type character found.");
                col_types_->append(c);
                col_names_->append(nullptr);
            }
        }

        /** Destructor */
        ~Schema() {
            delete col_types_;
            delete col_names_;
            delete row_names_;
        }
        
        /** Add a column of the given type and name (can be nullptr), name
         *  is external. Names are expectd to be unique, duplicates result
         *  in undefined behavior. */
        void add_column(char typ, String* name) {
            col_types_->append(typ);
            if (name != nullptr) {
                exit_if_not(!col_names_->contains(name), "Duplicate column name given.");
            }
            col_names_->append(name);
        }
        
        /** Add a row with a name (possibly nullptr), name is external.  Names are
         *  expectd to be unique, duplicates result in undefined behavior. */
        void add_row(String* name) {
            if (name != nullptr) {
                exit_if_not(!row_names_->contains(name), "Duplicate row name given.");
            }
            row_names_->append(name);
        }
        
        /** Return name of row at idx; nullptr indicates no name. An idx >= length
         *  is undefined. */
        String* row_name(size_t idx) {
            exit_if_not(idx < length(), "Row name index out of bounds.");
            return dynamic_cast<String*>(row_names_->get(idx));
        }
        
        /** Return name of column at idx; nullptr indicates no name given.
         *  An idx >= width is undefined.*/
        String* col_name(size_t idx) {
            exit_if_not(idx < width(), "Column name index out of bounds.");
            return dynamic_cast<String*>(col_names_->get(idx));
        }
        
        /** Return type of column at idx. An idx >= width is undefined. */
        char col_type(size_t idx) {
            exit_if_not(idx < width(), "Column type index out of bounds.");
            return col_types_->get(idx);
        }
        
        /** Given a column name return its index, or -1. */
        int col_idx(const char* name) {
            String* s = new String(name);
            int idx = col_names_->index_of(s);
            delete s;
            return idx;
        }
        
        /** Given a row name return its index, or -1. */
        int row_idx(const char* name) {
            String* s = new String(name);
            int idx = row_names_->index_of(s);
            delete s;
            return idx;
        }
        
        /** The number of columns */
        size_t width() {
            return col_types_->size();
        }
        
        /** The number of rows */
        size_t length() {
            return row_names_->size();
        }

        /** Getter for the array of column types */
        IntArray* get_types() {
            return col_types_;
        }

        /** Getter for the array of column names */
        Array* get_col_names() {
            return col_names_;
        }

        /** Getter for the array of row names */
        Array* get_row_names() {
            return row_names_;
        }

        void clear_row_names() {
            delete row_names_;
            row_names_ = new Array();
        }
};