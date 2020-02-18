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

        /**
         * Private function that deletes unecessary memory once enough items
         * have been removed from the Array.
         */
        // void deallocate_() {
        //     // Exponentially increasing the capacity each time for scalability.
        //     Object** new_arr = new Object*[capacity_ / 2];
        //     capacity_ /= 2;

        //     Object** old_arr = objects_;
        //     for (int i = 0; i < size_; i++) {
        //         new_arr[i] = old_arr[i];
        //     }
        //     delete[] old_arr;
        //     objects_ = new_arr;
        // }
        
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
        
        // Inserts val at index, so that array.get(index) will return val.
        // Pushes all elements after index down by 1.
        // If index is >= size(), is equivalent to calling append(val).
        // void insert(Object* val, size_t index) {
        //     assert(index >= 0);
        //     if (index >= size_) {
        //         append(val);
        //         return;
        //     }
            
        //     if (size_ + 1 > capacity_) reallocate_();
        //     for (int i = size_; i > index; i--) {
        //         objects_[i] = objects_[i - 1];
        //     }
        //     objects_[index] = val;
        //     size_++;
        // }
        
        // Inserts every element of vals into the array.
        // Pushes all elements after index down by the length of the array.
        // If vals is null, does nothing.
        // If index is >= size(), is equivalent to calling append_all(vals).
        // void insert_all(Array* vals, size_t index) {
        //     assert(index >= 0);
        //     if (index >= size_) {
        //         append_all(vals);
        //         return;
        //     }

        //     size_t vals_size = vals->size();
        //     while (size_ + vals_size > capacity_) {
        //         reallocate_();
        //     }
        //     int i;
        //     for (i = size_ - 1; i >= index; i--) {
        //         objects_[i + vals_size] = objects_[i];
        //     }
        //     int j = vals_size - 1;
        //     for (i = index + vals_size - 1; i >= index; i--) {
        //         objects_[i] = vals->get(j);
        //         j--;
        //     }
        //     size_ += vals_size;
        // }
        
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
        
        // Removes the element at index.
        // Pulls all elements after index up by 1.
        // If index is >= size(), does nothing and returns NULL.
        // Object* remove(size_t index) {
        //     assert(index >= 0);
        //     if (index >= size_) return NULL;

        //     Object* remove_me = objects_[index];
        //     for (int i = index; i < size_ - 1; i++) {
        //         objects_[i] = objects_[i + 1];
        //     }
        //     size_--;
        //     if (size_ < capacity_ / 2) deallocate_();
        //     return remove_me;
        // }
        
        // Removes the elements from start to end and returns a 
        //    new Array containing the elements. 
        //    Start is inclusive, end is exclusive
        // Pulls all elements after start up by (end - start).
        // If end <= start, does nothing and returns NULL.
        // Else if start >= size(), does nothing and returns NULL.
        // Else if end > size(), acts as if end == size().
        // Array* remove_range(size_t start, size_t end) {
        //     assert(start >= 0);
        //     assert(end >= 0);
        //     if (end <= start || start >= size_) return NULL;
        //     if (end > size_) end = size_;

        //     Array* removed = new Array();
        //     int new_index;
        //     for (int i = start; i < end; i++) {
        //         removed->append(objects_[i]);
        //         new_index = i + end - start;
        //         if (new_index < size_) {
        //             objects_[i] = objects_[new_index];
        //         }
        //     }
        //     size_ -= end - start;
        //     if (size_ < capacity_ / 2) deallocate_();
        //     return removed;
        // }
        
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
        
        // Clears all the elements in this array, 'delete' ing them.
        // void clear() {
        //     if (size_ > 0) {
        //         if (capacity_ > INITIAL_CAPACITY) {
        //             delete[] objects_;
        //             capacity_ = INITIAL_CAPACITY;
        //             objects_ = new Object*[capacity_];
        //         }
        //         size_ = 0;
        //     }
        // }
        
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

        /*
         * Private function that deletes unecessary memory once enough items
         * have been removed from the Array.
         */
        // void deallocate_() {
        //     // Exponentially increasing the capacity each time for scalability.
        //     bool** new_outer_arr = new bool*[capacity_ - INITIAL_OUTER_CAPACITY];

        //     bool** old_outer_arr = bools_;
        //     for (int i = 0; i < capacity_; i++) {
        //         new_outer_arr[i] = old_outer_arr[i];
        //     }
        //     delete[] old_outer_arr;
        //     bools_ = new_outer_arr;

        //     capacity_ -= INITIAL_OUTER_CAPACITY;
        // }
    
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
        
        // Inserts val at index, so that array.get(index) will return val.
        // Pushes all elements after index down by 1.
        // If index is >= size(), is equivalent to calling append(val).
        // void insert(bool val, size_t index) {
        //     assert(index >= 0);
        //     if (index >= size_) {
        //         append(val);
        //         return;
        //     }
            
        //     if (size_ + 1 > capacity_) reallocate_();
        //     for (int i = size_; i > index; i--) {
        //         bools_[i] = bools_[i - 1];
        //     }
        //     bools_[index] = val;
        //     size_++;
        // }
        
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
        
        // Removes the element at index. Returns the previous element.
        // Pulls all elements after index up by 1.
        // If index is >= size(), does nothing and returns undefined.
        // bool remove(size_t index) {
        //     if (index >= size_) return NULL;

        //     bool remove_me = bools_[index];
        //     for (int i = index; i < size_ - 1; i++) {
        //         bools_[i] = bools_[i + 1];
        //     }
        //     size_--;
        //     if (size_ < capacity_ / 2) deallocate_();
        //     return remove_me;
        // }
        
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
        
        // Removes all elements.
        // void clear() {
        //     if (size_ > 0) {
        //         delete[] bools_;
        //         capacity_ = INITIAL_CAPACITY;
        //         bools_ = new bool[capacity_];
        //         size_ = 0;
        //     }
        // }
        
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

        /*
         * Private function that deletes unecessary memory once enough items
         * have been removed from the IntArray.
         */
        // void deallocate_() {
        //     int* new_arr = new int[capacity_ / 2];
        //     capacity_ /= 2;

        //     int* old_arr = ints_;
        //     for (int i = 0; i < size_; i++) {
        //         new_arr[i] = old_arr[i];
        //     }
        //     delete[] old_arr;
        //     ints_ = new_arr;
        // }

        
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
        
        // Inserts val at index, so that array.get(index) will return val.
        // Pushes all elements after index down by 1.
        // If index is >= size(), is equivalent to calling append(val).
        // void insert(int val, size_t index) {
        //     assert(index >= 0);
        //     if (index >= size_) {
        //         append(val);
        //         return;
        //     }
            
        //     if (size_ + 1 > capacity_) reallocate_();
        //     for (int i = size_; i > index; i--) {
        //         ints_[i] = ints_[i - 1];
        //     }
        //     ints_[index] = val;
        //     size_++;
        // }
        
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
        
        // Removes the element at index. Returns the previous element.
        // Pulls all elements after index up by 1.
        // If index is >= size(), does nothing and returns undefined.
        // int remove(size_t index) {
        //     if (index >= size_) {
        //         printf("Index out of bounds, returning -1\n");
        //         return -1;
        //     }

        //     int remove_me = ints_[index];
        //     for (int i = index; i < size_ - 1; i++) {
        //         ints_[i] = ints_[i + 1];
        //     }
        //     size_--;
        //     if (size_ < capacity_ / 2) deallocate_();
        //     return remove_me;
        // }
        
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
        
        // Removes all elements.
        // void clear() {
        //     if (size_ > 0) {
        //         delete[] ints_;
        //         capacity_ = INITIAL_CAPACITY;
        //         ints_ = new int[capacity_];
        //         size_ = 0;
        //     }
        // }
        
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

        /*
         * Private function that deletes unecessary memory once enough items
         * have been removed from the FloatArray.
         */
        // void deallocate_() {
        //     // Exponentially increasing the capacity each time for scalability.
        //     float* new_arr = new float[capacity_ / 2];
        //     capacity_ /= 2;

        //     float* old_arr = floats_;
        //     for (int i = 0; i < size_; i++) {
        //         new_arr[i] = old_arr[i];
        //     }
        //     delete[] old_arr;
        //     floats_ = new_arr;
        // }

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
        
        // Inserts val at index, so that array.get(index) will return val.
        // Pushes all elements after index down by 1.
        // If index is >= size(), is equivalent to calling append(val).
        // void insert(float val, size_t index) {
        //     assert(index >= 0);
        //     if (index >= size_) {
        //         append(val);
        //         return;
        //     }
            
        //     if (size_ + 1 > capacity_) reallocate_();
        //     for (int i = size_; i > index; i--) {
        //         floats_[i] = floats_[i - 1];
        //     }
        //     floats_[index] = val;
        //     size_++;
        // }
        
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
        
        // Removes the element at index. Returns the previous element.
        // Pulls all elements after index up by 1.
        // If index is >= size(), does nothing and returns undefined.
        // float remove(size_t index) {
        //     if (index >= size_) {
        //         printf("Index out of bounds, returning -1\n");
        //         return -1;
        //     }

        //     float remove_me = floats_[index];
        //     for (int i = index; i < size_ - 1; i++) {
        //         floats_[i] = floats_[i + 1];
        //     }
        //     size_--;
        //     if (size_ < capacity_ / 2) deallocate_();
        //     return remove_me;
        // }
        
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
        
        // Removes all elements.
        // void clear() {
        //     if (size_ > 0) {
        //         delete[] floats_;
        //         capacity_ = INITIAL_CAPACITY;
        //         floats_ = new float[capacity_];
        //         size_ = 0;
        //     }
        // }
        
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
