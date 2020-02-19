#pragma once

#include "string.h"
#include <stdbool.h>
#include <assert.h>

#define INITIAL_OUTER_CAPACITY 16
#define INNER_CAPACITY 8

/**
 * Represents an array (Java: List) of objects.
 * In order to have constant time lookup and avoid copying the payload of the array,
 * this data structure is an array of array pointers, essentially a 2D array.
 * The inner arrays have a fixed length and when the outer array runs out of space,
 * a new array is allocated and the inner array pointers are transferred to it.
 * 
 * @author Spencer LaChance <lachance.s@husky.neu.edu>
 * @author David Mberingabo <mberingabo.d@husky.neu.edu>
 */
class Array : public Object {
    public:
        Object*** objects_;   
        int size_;
        // Number of inner arrays that we have space for
        int outer_capacity_; 
        // Number of inner arrays that have been initialized
        int array_count_;
    
        /**
         * Initialize an empty Array.
         */
        Array() {
            size_ = 0;
            outer_capacity_ = INITIAL_OUTER_CAPACITY;
            array_count_ = 1;
            objects_ = new Object**[outer_capacity_];
            objects_[0] = new Object*[INNER_CAPACITY];
        }

        /**
         * Destructor for a Array
         */
        ~Array() {
            delete[] objects_;
        }

        /**
         * Private function that reallocates more space for the array
         * once it fills up.
         */
        void reallocate_() {
            Object*** new_outer_arr = new Object**[outer_capacity_ + INITIAL_OUTER_CAPACITY];

            Object*** old_outer_arr = objects_;
            for (int i = 0; i < array_count_; i++) {
                new_outer_arr[i] = old_outer_arr[i];
            }
            delete[] old_outer_arr;
            objects_ = new_outer_arr;

            outer_capacity_ += INITIAL_OUTER_CAPACITY;
        }
        
        // Appends val to the end of the array.
        void append(Object* val) {
            // If all of the inner arrays are full, allocate more memory for the outer one.
            if (size_ + 1 > outer_capacity_ * INNER_CAPACITY) reallocate_();
            // If the last inner array is full, create a new one and add val to it.
            if (size_ + 1 > array_count_ * INNER_CAPACITY) {
                objects_[array_count_] = new Object*[INNER_CAPACITY];
                objects_[array_count_][0] = val;
                array_count_++;
            } else {
                objects_[array_count_ - 1][size_ % INNER_CAPACITY] = val;
            }
            size_++;
        }
        
        // Appends every element of vals to the end of the array.
        // If vals is null, does nothing.
        void append_all(Array* vals) {
            if (vals == NULL) return;
            for (int i = 0; i < vals->size(); i++) {
                Object* val = vals->get(i);
                if (val == nullptr) {
                    append(nullptr);
                } else {
                    append(val->clone());
                }
            }
        }
        
        // Sets the element at index to val.
        // If index == size(), appends to the end of the array.
        void set(Object* val, size_t index) {
            assert(index >= 0);
            assert(index <= size_);

            if (index == size_) {
                append(val);
                return;
            }

            int outer_idx = index / INNER_CAPACITY;
            int inner_idx = index % INNER_CAPACITY;
            objects_[outer_idx][inner_idx] = val;
        }
        
        // Gets the element at the given index.
        Object* get(size_t index) {
            assert(index >= 0);
            assert(index < size_);
            int outer_idx = index / INNER_CAPACITY;
            int inner_idx = index % INNER_CAPACITY;
            return objects_[outer_idx][inner_idx];
        }
        
        // Returns if this array contains obj, using obj->equals().
        // If obj is null, uses == .
        bool contains(Object* obj) {
            // count is the total number of items read in the array so far
            int count = 0;
            for (int i = 0; i < array_count_ && count < size_; i++) {
                for (int j = 0; j < INNER_CAPACITY && count < size_; j++) {
                    int inner_idx = j % INNER_CAPACITY;
                    if (obj == NULL || objects_[i][inner_idx] == NULL) {
                        if (objects_[i][inner_idx] == obj) return true;
                    } else {
                        if (objects_[i][inner_idx]->equals(obj)) return true;
                    }
                    count++;
                }
            }
            return false;
        }
        
        // Returns the first index of obj, using obj->equals().
        // If obj is null, uses == .
        // If obj does not exist in the array, returns -1.
        size_t index_of(Object* obj) {
            // count is the total number of items read in the array so far
            int count = 0;
            for (int i = 0; i < array_count_ && count < size_; i++) {
                for (int j = 0; j < INNER_CAPACITY && count < size_; j++) {
                    int inner_idx = j % INNER_CAPACITY;
                    if (obj == NULL || objects_[i][inner_idx] == NULL) {
                        if (objects_[i][inner_idx] == obj) return count;
                    } else {
                        if (objects_[i][inner_idx]->equals(obj)) return count;
                    }
                    count++;
                } 
            }
            return -1;
        }
        
        // Returns the number of elements in this array. 
        size_t size() {
            return size_;
        }

        // Inherited from Object
        // Is this Array equal to the given Object?
        bool equals(Object* o) {
            Array* other = dynamic_cast<Array*>(o);
            if (other == nullptr) return false;
            if (size_ == 0) return other->size() == 0;

            // count is the total number of items read in the array so far
            int count = 0;
            for (int i = 0; i < array_count_ && count < size_; i++) {
                for (int j = 0; j < INNER_CAPACITY && count < size_; j++) {
                    int inner_idx = j % INNER_CAPACITY;
                    if (!(objects_[i][inner_idx] == NULL && other->get(count) == NULL)) return false;
                    if (!objects_[i][inner_idx]->equals(other->get(count))) return false;
                    count++;
                } 
            }

            return true;
        }
};

/**
 * Represents an array (Java: List) of booleans.
 * In order to have constant time lookup and avoid copying the payload of the array,
 * this data structure is an array of array pointers, essentially a 2D array.
 * The inner arrays have a fixed length and when the outer array runs out of space,
 * a new array is allocated and the inner array pointers are transferred to it.
 * 
 * @author Spencer LaChance <lachance.s@husky.neu.edu>
 * @author David Mberingabo <mberingabo.d@husky.neu.edu>
 */
class BoolArray : public Object {
    public:
        bool** bools_;
        int size_;
        // Number of inner arrays that we have space for
        int outer_capacity_; 
        // Number of inner arrays that have been initialized
        int array_count_;

        /**
         * Constructor for a BoolArray.
         * 
        */ 
        BoolArray() {
            size_ = 0;
            outer_capacity_ = INITIAL_OUTER_CAPACITY;
            array_count_ = 1;
            bools_ = new bool*[outer_capacity_];
            bools_[0] = new bool[INNER_CAPACITY];
        } 

        /**
         * Destructor for a BoolArray.
         */ 
        ~BoolArray() {
            delete[] bools_;
        }

        /*
         * Private function that reallocates more space for the array
         * once it fills up.
         */
        void reallocate_() {
            bool** new_outer_arr = new bool*[outer_capacity_ + INITIAL_OUTER_CAPACITY];

            bool** old_outer_arr = bools_;
            for (int i = 0; i < array_count_; i++) {
                new_outer_arr[i] = old_outer_arr[i];
            }
            delete[] old_outer_arr;
            bools_ = new_outer_arr;

            outer_capacity_ += INITIAL_OUTER_CAPACITY;
        }
    
        // Appends val onto the end of the array
        void append(bool val) {
            // If all of the inner arrays are full, allocate more memory for the outer one.
            if (size_ + 1 > outer_capacity_ * INNER_CAPACITY) reallocate_();
            // If the last inner array is full, create a new one and add val to it.
            if (size_ + 1 > array_count_ * INNER_CAPACITY) {
                bools_[array_count_] = new bool[INNER_CAPACITY];
                bools_[array_count_][0] = val;
                array_count_++;
            } else {
                bools_[array_count_ - 1][size_ % INNER_CAPACITY] = val;
            }
            size_++;
        }
        
        // Appends every element of vals to the end of the array.
        // If vals is null, does nothing.
        void append_all(BoolArray* vals) {
            if (vals == NULL) return;
            for (int i = 0; i < vals->size(); i++) {
                append(vals->get(i));
            }
        }
        
        // Sets the element at index to val.
        // If index == size(), appends to the end of the array.
        void set(bool val, size_t index) { 
            assert(index >= 0);
            assert(index <= size_);

            if (index == size_) {
                append(val);
                return;
            }

            int outer_idx = index / INNER_CAPACITY;
            int inner_idx = index % INNER_CAPACITY;
            bools_[outer_idx][inner_idx] = val;
        }
        
        // Gets the element at the given index.
        bool get(size_t index) { 
            assert(index >= 0);
            assert(index < size_);
            int outer_idx = index / INNER_CAPACITY;
            int inner_idx = index % INNER_CAPACITY;
            return bools_[outer_idx][inner_idx];
        }
        
        // Returns if the array contains val.
        bool contains(bool val) {
            // count is the total number of items read in the array so far
            int count = 0;
            for (int i = 0; i < array_count_ && count < size_; i++) {
                for (int j = 0; j < INNER_CAPACITY && count < size_; j++) {
                    if (bools_[i][j % INNER_CAPACITY] == val) return true;
                }
                count++;
            }
            return false;
        }
        
        // Returns the first index of the val.
        // If val does not exist in the array, returns -1.
        size_t index_of(bool val) {
            // count is the total number of items read in the array so far
            int count = 0;
            for (int i = 0; i < array_count_ && count < size_; i++) {
                for (int j = 0; j < INNER_CAPACITY && count < size_; j++) {
                    if (bools_[i][j % INNER_CAPACITY] == val) return count;
                }
                count++;
            }
            return -1;
        }
        
        // Returns the number of elements.
        size_t size() {
            return size_;
        }

        // Inherited from Object
        // Is this Array equal to the given Object?
        bool equals(Object* o) {
            BoolArray* other = dynamic_cast<BoolArray*>(o);
            if (other == nullptr) return false;
            if (size_ == 0) return other->size() == 0;

            // count is the total number of items read in the array so far
            int count = 0;
            for (int i = 0; i < array_count_ && count < size_; i++) {
                for (int j = 0; j < INNER_CAPACITY && count < size_; j++) {
                    int inner_idx = j % INNER_CAPACITY;
                    if (!(bools_[i][inner_idx] == other->get(count))) return false;
                    count++;
                } 
            }

            return true;
        }
};

/**
 * Represents an array (Java: List) of integers.
 * In order to have constant time lookup and avoid copying the payload of the array,
 * this data structure is an array of array pointers, essentially a 2D array.
 * The inner arrays have a fixed length and when the outer array runs out of space,
 * a new array is allocated and the inner array pointers are transferred to it.
 * 
 * @author Spencer LaChance <lachance.s@husky.neu.edu>
 * @author David Mberingabo <mberingabo.d@husky.neu.edu>
 */
class IntArray : public Object {
    public:
        int** ints_;
        int size_;
        // Number of inner arrays that we have space for
        int outer_capacity_; 
        // Number of inner arrays that have been initialized
        int array_count_;

        /**
         * Constructor for an IntArray.
         * 
        */ 
        IntArray() {
            size_ = 0;
            outer_capacity_ = INITIAL_OUTER_CAPACITY;
            array_count_ = 1;
            ints_ = new int*[outer_capacity_];
            ints_[0] = new int[INNER_CAPACITY];
        } 

        /**
         * Destructor for an IntArray.
         * 
        */ 
        ~IntArray() {
            delete[] ints_;
        }

        /*
         * Private function that reallocates more space for the IntArray
         * once it fills up.
         */
        void reallocate_() {
            int** new_outer_arr = new int*[outer_capacity_ + INITIAL_OUTER_CAPACITY]; 

            int** old_outer_arr = ints_;
            for (int i = 0; i < array_count_; i++) {
                new_outer_arr[i] = old_outer_arr[i];
            }
            delete[] old_outer_arr;
            ints_ = new_outer_arr;

            outer_capacity_ += INITIAL_OUTER_CAPACITY;
        }
        
        // Appends val onto the end of the array
        void append(int val) {
            // If all of the inner arrays are full, allocate more memory for the outer one.
            if (size_ + 1 > outer_capacity_ * INNER_CAPACITY) reallocate_();
            // If the last inner array is full, create a new one and add val to it.
            if (size_ + 1 > array_count_ * INNER_CAPACITY) {
                ints_[array_count_] = new int[INNER_CAPACITY];
                ints_[array_count_][0] = val;
                array_count_++;
            } else {
                ints_[array_count_ - 1][size_ % INNER_CAPACITY] = val;
            }
            size_++;
        }
        
        // Appends every element of vals to the end of the array.
        // If vals is null, does nothing.
        void append_all(IntArray* vals) {
            if (vals == NULL) return;
            for (int i = 0; i < vals->size(); i++) {
                append(vals->get(i));
            }
        }
        
        // Sets the element at index to val.
        // If index == size(), appends to the end of the array.
        void set(int val, size_t index) {
            assert(index >= 0);
            assert(index <= size_);

            if (index == size_) {
                append(val);
                return;
            }

            int outer_idx = index / INNER_CAPACITY;
            int inner_idx = index % INNER_CAPACITY;
            ints_[outer_idx][inner_idx] = val;
        }
        
        // Gets the element at index.
        // If index is >= size(), does nothing and returns undefined.
        int get(size_t index) {
            assert(index >= 0);
            assert(index < size_);
            int outer_idx = index / INNER_CAPACITY;
            int inner_idx = index % INNER_CAPACITY;
            return ints_[outer_idx][inner_idx];
        }
        
        // Returns if the array contains val.
        bool contains(int val) {
            // count is the total number of items read in the array so far
            int count = 0;
            for (int i = 0; i < array_count_ && count < size_; i++) {
                for (int j = 0; j < INNER_CAPACITY && count < size_; j++) {
                    if (ints_[i][j % INNER_CAPACITY] == val) return true;
                }
                count++;
            }
            return false;
        }
        
        
        // Returns the first index of the val.
        // If val does not exist in the array, returns -1.
        size_t index_of(int val) {
            // count is the total number of items read in the array so far
            int count = 0;
            for (int i = 0; i < array_count_ && count < size_; i++) {
                for (int j = 0; j < INNER_CAPACITY && count < size_; j++) {
                    if (ints_[i][j % INNER_CAPACITY] == val) return count;
                }
                count++;
            }
            return -1;
        }
        
        // Returns the number of elements.
        size_t size() {
            return size_;
        }

        // Inherited from Object
        // Is this Array equal to the given Object?
        bool equals(Object* o) {
            IntArray* other = dynamic_cast<IntArray*>(o);
            if (other == nullptr) return false;
            if (size_ == 0) return other->size() == 0;

            // count is the total number of items read in the array so far
            int count = 0;
            for (int i = 0; i < array_count_ && count < size_; i++) {
                for (int j = 0; j < INNER_CAPACITY && count < size_; j++) {
                    int inner_idx = j % INNER_CAPACITY;
                    if (!(ints_[i][inner_idx] == other->get(count))) return false;
                    count++;
                } 
            }

            return true;
        }
};

/**
 * Represents an array (Java: List) of floats.
 * In order to have constant time lookup and avoid copying the payload of the array,
 * this data structure is an array of array pointers, essentially a 2D array.
 * The inner arrays have a fixed length and when the outer array runs out of space,
 * a new array is allocated and the inner array pointers are transferred to it.
 * 
 * @author Spencer LaChance <lachance.s@husky.neu.edu>
 * @author David Mberingabo <mberingabo.d@husky.neu.edu>
 */
class FloatArray : public Object {
    public:
        float** floats_;
        int size_;
        // Number of inner arrays that we have space for
        int outer_capacity_; 
        // Number of inner arrays that have been initialized
        int array_count_;

        /**
         * Constructor for a FloatArray.
         * 
        */ 
        FloatArray() {
            size_ = 0;
            outer_capacity_ = INITIAL_OUTER_CAPACITY;
            array_count_ = 1;
            floats_ = new float*[outer_capacity_];
            floats_[0] = new float[INNER_CAPACITY];
        }

        /**
         * Destructor for a FloatArray.
         * 
        */ 
        ~FloatArray() {
            delete[] floats_;
        }
        
        /*
         * Private function that reallocates more space for the FloatArray
         * once it fills up.
         */
        void reallocate_() {
            float** new_outer_arr = new float*[outer_capacity_ + INITIAL_OUTER_CAPACITY];

            float** old_outer_arr = floats_;
            for (int i = 0; i < array_count_; i++) {
                new_outer_arr[i] = old_outer_arr[i];
            }
            delete[] old_outer_arr;
            floats_ = new_outer_arr;

            outer_capacity_ += INITIAL_OUTER_CAPACITY;
        }

        // Appends val onto the end of the array
        void append(float val) {
            // If all of the inner arrays are full, allocate more memory for the outer one.
            if (size_ + 1 > outer_capacity_ * INNER_CAPACITY) reallocate_();
            // If the last inner array is full, create a new one and add val to it.
            if (size_ + 1 > array_count_ * INNER_CAPACITY) {
                floats_[array_count_] = new float[INNER_CAPACITY];
                floats_[array_count_][0] = val;
                array_count_++;
            } else {
                floats_[array_count_ - 1][size_ % INNER_CAPACITY] = val;
            }
            size_++;
        }
        
        // Appends every element of vals to the end of the array.
        // If vals is null, does nothing.
        void append_all(FloatArray* vals) {
            if (vals == NULL) return;
            for (int i = 0; i < vals->size(); i++) {
                append(vals->get(i));
            }
        }
        
        // Sets the element at index to val.
        // If index == size(), appends to the end of the array.
        void set(float val, size_t index) {
            assert(index >= 0);
            assert(index <= size_);

            if (index == size_) {
                append(val);
                return;
            }

            int outer_idx = index / INNER_CAPACITY;
            int inner_idx = index % INNER_CAPACITY;
            floats_[outer_idx][inner_idx] = val;
        }
        
        // Gets the element at index.
        float get(size_t index) {
            assert(index >= 0);
            assert(index < size_);
            int outer_idx = index / INNER_CAPACITY;
            int inner_idx = index % INNER_CAPACITY;
            return floats_[outer_idx][inner_idx];
        }
        
        // Returns if the array contains val.
        bool contains(float val) {
            // count is the total number of items read in the array so far
            int count = 0;
            for (int i = 0; i < array_count_ && count < size_; i++) {
                for (int j = 0; j < INNER_CAPACITY && count < size_; j++) {
                    if (floats_[i][j % INNER_CAPACITY] == val) return true;
                }
                count++;
            }
            return false;
        }
        
        // Returns the first index of the val.
        // If val does not exist in the array, 
        //    returns a value greater than size().
        size_t index_of(float val) {
            // count is the total number of items read in the array so far
            int count = 0;
            for (int i = 0; i < array_count_ && count < size_; i++) {
                for (int j = 0; j < INNER_CAPACITY && count < size_; j++) {
                    if (floats_[i][j % INNER_CAPACITY] == val) return count;
                }
                count++;
            }
            return -1;
        }
        
        // Returns the number of elements.
        size_t size() {
            return size_;
        }

        // Inherited from Object
        // Is this Array equal to the given Object?
        bool equals(Object* o) {
            FloatArray* other = dynamic_cast<FloatArray*>(o);
            if (other == nullptr) return false;
            if (size_ == 0) return other->size() == 0;

            // count is the total number of items read in the array so far
            int count = 0;
            for (int i = 0; i < array_count_ && count < size_; i++) {
                for (int j = 0; j < INNER_CAPACITY && count < size_; j++) {
                    int inner_idx = j % INNER_CAPACITY;
                    if (!(floats_[i][inner_idx] == other->get(count))) return false;
                    count++;
                } 
            }

            return true;
        }
};
