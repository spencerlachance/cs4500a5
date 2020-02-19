//lang:CwC

#pragma once

#include "object.h"
#include "string.h"
#include "array.h"
#include <stdarg.h>

class IntColumn;
class BoolColumn;
class FloatColumn;
class StringColumn;

/**************************************************************************
 * Column ::
 * Represents one column of a data frame which holds values of a single type.
 * This abstract class defines methods overriden in subclasses. There is
 * one subclass per element type. Columns are mutable, equality is pointer
 * equality. 
 * 
 * @author Spencer LaChance <lachance.s@husky.neu.edu>
 * @author David Mberingabo <mberingabo.d@husky.neu.edu>
 */
class Column : public Object {
    public:
        char type_;

        /** Type converters: Return same column under its actual type, or
         *  nullptr if of the wrong type.  */
        virtual IntColumn* as_int() = 0;
        virtual BoolColumn*  as_bool() = 0;
        virtual FloatColumn* as_float() = 0;
        virtual StringColumn* as_string() = 0;

        /** Type appropriate push_back methods. Calling the wrong method is
        * undefined behavior. **/
        virtual void push_back(int val) = 0;
        virtual void push_back(bool val) = 0;
        virtual void push_back(float val) = 0;
        virtual void push_back(String* val) = 0;

        /** Returns the number of elements in the column. */
        virtual size_t size() = 0;

        /** Clones the column */
        virtual Column* clone() = 0;

        /** Appends a default value that represents a missing field */
        virtual void append_missing() = 0;

        /** Return the type of this column as a char: 'S', 'B', 'I' and 'F'. */
        char get_type() {
            return type_;
        }
};
 
/*************************************************************************
 * IntColumn::
 * Holds int values.
 * 
 * @author Spencer LaChance <lachance.s@husky.neu.edu>
 * @author David Mberingabo <mberingabo.d@husky.neu.edu>
 */
class IntColumn : public Column {
    public:
        IntArray* ints_;
        
        /** Constructs an empty IntColumn */
        IntColumn() {
            ints_ = new IntArray();
            type_ = 'I';
        }

        /** Constructs an IntColumn initialized with the given integers. */
        IntColumn(int n, ...) {
            ints_ = new IntArray();
            type_ = 'I';
            va_list vl;
            va_start(vl, n);
            for (int i = 0; i < n; i++) {
                ints_->append(va_arg(vl, int));
            }
            va_end(vl);
        }

        /** Destructor */
        ~IntColumn() {
            delete ints_;
        }

        /** Adds the given field to the end of the column. */
        void push_back(int val) {
            ints_->append(val);
        }

        /** Does nothing because a boolean cannot be added to this column. */
        void push_back(bool val) {
            exit_if_not(false, "Cannot call this function on an IntColumn.");
            return;
        }

        /** Does nothing because a float cannot be added to this column. */
        void push_back(float val) {
            exit_if_not(false, "Cannot call this function on an IntColumn.");
            return;
        }

        /** Does nothing because a string cannot be added to this column. */
        void push_back(String* val) {
            exit_if_not(false, "Cannot call this function on an IntColumn.");
            return;
        }

        /** Gets the int at the specified index. */
        int get(size_t idx) {
            return ints_->get(idx);
        }

        /** Returns this IntColumn. */
        IntColumn* as_int() {
            return this;
        }

        /** Returns nullptr because this is not a BoolColumn. */
        BoolColumn* as_bool() {
            exit_if_not(false, "Cannot call this function on an IntColumn.");
            return nullptr;
        }

        /** Returns nullptr because this is not a FloatColumn. */
        FloatColumn* as_float() {
            exit_if_not(false, "Cannot call this function on an IntColumn.");
            return nullptr;
        }

        /** Returns nullptr because this is not a StringColumn. */
        StringColumn* as_string() {
            exit_if_not(false, "Cannot call this function on an IntColumn.");
            return nullptr;
        }

        /** Set value at idx. An out of bound idx is undefined.  */
        void set(size_t idx, int val) {
            ints_->set(val, idx);
        }

        /** Returns the number of fields in this IntColumn */
        size_t size() {
            return ints_->size();
        }

        /** Getter for this column's underlying array of fields. */
        IntArray* get_fields() {
            return ints_;
        }

        /** Returns a clone of this IntColumn. */
        Column* clone() {
            IntColumn* clone = new IntColumn();
            clone->get_fields()->append_all(ints_);
            return clone;
        }

        /** Appends a default value that represents a missing field */
        void append_missing() {
            push_back(0);
        }
};
 
/*************************************************************************
 * BoolColumn::
 * Holds boolean values.
 * 
 * @author Spencer LaChance <lachance.s@husky.neu.edu>
 * @author David Mberingabo <mberingabo.d@husky.neu.edu>
 */
class BoolColumn : public Column {
    public:
        BoolArray* bools_;
        
        /** Constructs an empty BoolColumn */
        BoolColumn() {
            bools_ = new BoolArray();
            type_ = 'B';
        }

        /** Constructs an BoolColumn initialized with the given booleans. */
        BoolColumn(int n, ...) {
            bools_ = new BoolArray();
            type_ = 'B';
            va_list vl;
            va_start(vl, n);
            for (int i = 0; i < n; i++) {
                bools_->append(va_arg(vl, int));
            }
            va_end(vl);
        }

        /** Destructor */
        ~BoolColumn() {
            delete bools_;
        }

        /** Does nothing because an integer cannot be added to this column. */
        void push_back(int val) {
            exit_if_not(false, "Cannot call this function on a BoolColumn.");
            return;
        }

        /** Adds the given field to the end of this column. */
        void push_back(bool val) {
            bools_->append(val);
        }

        /** Does nothing because a float cannot be added to this column. */
        void push_back(float val) {
            exit_if_not(false, "Cannot call this function on a BoolColumn.");
            return;
        }

        /** Does nothing because a string cannot be added to this column. */
        void push_back(String* val) {
            exit_if_not(false, "Cannot call this function on a BoolColumn.");
            return;
        }

        /** Gets the boolean at the specified index. */
        bool get(size_t idx) {
            return bools_->get(idx);
        }

        /** Returns nullptr because this is not an IntColumn. */
        IntColumn* as_int() {
            exit_if_not(false, "Cannot call this function on a BoolColumn.");
            return nullptr;
        }

        /** Returns this BoolColumn. */
        BoolColumn* as_bool() {
            return this;
        }

        /** Returns nullptr because this is not a FloatColumn. */
        FloatColumn* as_float() {
            exit_if_not(false, "Cannot call this function on a BoolColumn.");
            return nullptr;
        }

        /** Returns nullptr because this is not a StringColumn. */
        StringColumn* as_string() {
            exit_if_not(false, "Cannot call this function on a BoolColumn.");
            return nullptr;
        }

        /** Set value at idx. An out of bound idx is undefined.  */
        void set(size_t idx, bool val) {
            bools_->set(val, idx);
        }

        /** Returns the number of fields in this BoolColumn */
        size_t size() {
            return bools_->size();
        }

        /** Getter for this column's underlying array of fields. */
        BoolArray* get_fields() {
            return bools_;
        }

        /** Returns a clone of this BoolColumn. */
        Column* clone() {
            BoolColumn* clone = new BoolColumn();
            clone->get_fields()->append_all(bools_);
            return clone;
        }

        /** Appends a default value that represents a missing field */
        void append_missing() {
            push_back(false);
        }
};
 
/*************************************************************************
 * FloatColumn::
 * Holds float values.
 * 
 * @author Spencer LaChance <lachance.s@husky.neu.edu>
 * @author David Mberingabo <mberingabo.d@husky.neu.edu>
 */
class FloatColumn : public Column {
    public:
        FloatArray* floats_;
        
        /** Constructs an empty FloatColumn */
        FloatColumn() {
            floats_ = new FloatArray();
            type_ = 'F';
        }

        /** Constructs an FloatColumn initialized with the given floats. */
        FloatColumn(int n, ...) {
            floats_ = new FloatArray();
            type_ = 'F';
            va_list vl;
            va_start(vl, n);
            for (int i = 0; i < n; i++) {
                floats_->append(va_arg(vl, double));
            }
            va_end(vl);
        }

        /** Destructor */
        ~FloatColumn() {
            delete floats_;
        }

        /** Does nothing because an integer cannot be added to this column. */
        void push_back(int val) {
            exit_if_not(false, "Cannot call this function on a FloatColumn.");
            return;
        }

        /** Does nothing because a boolean cannot be added to this column. */
        void push_back(bool val) {
            exit_if_not(false, "Cannot call this function on a FloatColumn.");
            return;
        }

        /** Adds the given field to the end of this column. */
        void push_back(float val) {
            floats_->append(val);
        }

        /** Does nothing because a string cannot be added to this column. */
        void push_back(String* val) {
            exit_if_not(false, "Cannot call this function on a FloatColumn.");
            return;
        }

        /** Gets the float at the specified index. */
        float get(size_t idx) {
            return floats_->get(idx);
        }

        /** Returns nullptr because this is not an IntColumn. */
        IntColumn* as_int() {
            exit_if_not(false, "Cannot call this function on a FloatColumn.");
            return nullptr;
        }

        /** Returns nullptr because this is not a BoolColumn. */
        BoolColumn* as_bool() {
            exit_if_not(false, "Cannot call this function on a FloatColumn.");
            return nullptr;
        }

        /** Returns this FloatColumn. */
        FloatColumn* as_float() {
            return this;
        }

        /** Returns nullptr because this is not a StringColumn. */
        StringColumn* as_string() {
            exit_if_not(false, "Cannot call this function on a FloatColumn.");
            return nullptr;
        }

        /** Set value at idx. An out of bound idx is undefined.  */
        void set(size_t idx, float val) {
            floats_->set(val, idx);
        }

        /** Returns the number of fields in this FloatColumn */
        size_t size() {
            return floats_->size();
        }

        /** Getter for this column's underlying array of fields. */
        FloatArray* get_fields() {
            return floats_;
        }

        /** Returns a clone of this FloatColumn. */
        Column* clone() {
            FloatColumn* clone = new FloatColumn();
            clone->get_fields()->append_all(floats_);
            return clone;
        }

        /** Appends a default value that represents a missing field */
        void append_missing() {
            push_back(0.0f);
        }
};
 
/*************************************************************************
 * StringColumn::
 * Holds string pointers. The strings are external.  Nullptr is a valid
 * value.
 */
class StringColumn : public Column {
    public:
        Array* strings_;
        
        /** Constructs an empty StringColumn */
        StringColumn() {
            strings_ = new Array();
            type_ = 'S';
        }

        /** Constructs a StringColumn and initializes it with the given strings. */
        StringColumn(int n, ...) {
            strings_ = new Array();
            type_ = 'S';
            va_list vl;
            va_start(vl, n);
            for (int i = 0; i < n; i++) {
                strings_->append(va_arg(vl, String*));
            }
            va_end(vl);
        }

        /** Destructor */
        ~StringColumn() {
            delete strings_;
        }

        /** Does nothing because an integer cannot be added to this column. */
        void push_back(int val) {
            exit_if_not(false, "Cannot call this function on a StringColumn.");
            return;
        }

        /** Does nothing because a boolean cannot be added to this column. */
        void push_back(bool val) {
            exit_if_not(false, "Cannot call this function on a StringColumn.");
            return;
        }

        /** Does nothing because a float cannot be added to this column. */
        void push_back(float val) {
            exit_if_not(false, "Cannot call this function on a StringColumn.");
            return;
        }

        /** Adds the given field to this column. */
        void push_back(String* val) {
            strings_->append(val);
        }

        /** Returns nullptr because this is not an IntColumn. */
        IntColumn* as_int() {
            exit_if_not(false, "Cannot call this function on a StringColumn.");
            return nullptr;
        }

        /** Returns nullptr because this is not a BoolColumn. */
        BoolColumn* as_bool() {
            exit_if_not(false, "Cannot call this function on a StringColumn.");
            return nullptr;
        }

        /** Returns nullptr becuase this is not a FloatColumn. */
        FloatColumn* as_float() {
            exit_if_not(false, "Cannot call this function on a StringColumn.");
            return nullptr;
        }

        /** Returns this StringColumn. */
        StringColumn* as_string() {
            return this;
        }

        /** Returns the string at idx; undefined on invalid idx.*/
        String* get(size_t idx) {
            return dynamic_cast<String*>(strings_->get(idx));
        }

        /** Acquire ownership of the string.  Out of bound idx is undefined. */
        void set(size_t idx, String* val) {
            strings_->set(val, idx);
        }

        /** Returns the number of fields in this StringColumn */
        size_t size() {
            return strings_->size();
        }

        /** Getter for this column's underlying array of fields. */
        Array* get_fields() {
            return strings_;
        }

        /** Returns a clone of this StringColumn. */
        Column* clone() {
            StringColumn* clone = new StringColumn();
            clone->get_fields()->append_all(strings_);
            return clone;
        }

        /** Appends a default value that represents a missing field */
        void append_missing() {
            push_back(nullptr);
        }
};