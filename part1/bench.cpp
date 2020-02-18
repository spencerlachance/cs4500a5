//lang::CwC

#include "modified_dataframe.h"
#include "parser_main.h"

#include <time.h>

/**
 * A Fielder that adds up every int it finds in a row.
 * 
 * @author David Mberingabo <mberingabo.d@husky.neu.edu>
 * @author Spencer LaChance <lachance.s@husky.neu.edu>
 */
class SumFielder : public Fielder {
    public:
        int total_;

        void start(size_t r) { total_ = 0; }
        void done() { }

        SumFielder(int total) {
            total_ = total;
        }
        ~SumFielder() { }

        void accept(bool b) { }
        void accept(float f) { }
        void accept(String* s) { }
        void accept(int i) {
            total_ += i;
        }

        int get_total() {
            return total_;
        }
};

/**
 * A Rower that adds up every int it finds in a row using a Fielder.
 * 
 * @author David Mberingabo <mberingabo.d@husky.neu.edu>
 * @author Spencer LaChance <lachance.s@husky.neu.edu>
 */
class SumRower : public Rower {
    public:
        SumFielder* sf_;
        int total_;

        SumRower() {
            total_ = 0;
            sf_ = new SumFielder(total_);
        }
        ~SumRower() { delete sf_; }

        bool accept(Row& r) {
            r.visit(r.get_idx(), *sf_);
            total_ += sf_->get_total();
        }

        int get_total() {
            return total_;
        }

        void join_delete(Rower* other) {
            SumRower* o = dynamic_cast<SumRower*>(other);
            total_ += o->get_total();
            delete o;
        }

        Object* clone() {
            return new SumRower();
        }
};

/**
 * A Fielder that increments every int and float and switches every boolean
 * it finds in a DataFrame and returns a new DataFrame with filled with the incremented values.
 * 
 * @author David Mberingabo <mberingabo.d@husky.neu.edu>
 * @author Spencer LaChance <lachance.s@husky.neu.edu>
 */
class IncrementFielder : public Fielder {
    public:
        DataFrame* df_;
        DataFrame* new_df_;
        Row* row_;
        Row* new_row_;
        // The index within the row we're traversing.
        size_t col_index_;

        IncrementFielder(DataFrame* df) {
            df_ = df;
            Schema* new_schema = new Schema(df_->get_schema());
            new_df_ = new DataFrame(*new_schema);
        }
        ~IncrementFielder() { }

        void start(size_t r) {
            row_ = new Row(df_->get_schema());
            df_->fill_row(r, *row_);
            col_index_ = 0;
            new_row_ = new Row(df_->get_schema());
        }
        void done() {
            new_df_->add_row(*new_row_);
            delete row_;
            delete new_row_;
        }

        void accept(bool b) { 
            new_row_->set(col_index_, !b);
            col_index_++;
        }
        void accept(float f) {
            new_row_->set(col_index_, f + 1);
            col_index_++;
        }
        void accept(String* s) { }
        void accept(int i) {
            new_row_->set(col_index_, i + 1);
            col_index_++;
        }

        DataFrame* get_new_df() {
            return new_df_;
        }
};

/**
 * A Rower that increments every int and float and switches every boolean
 * it finds in a row using a fielder.
 * 
 * @author David Mberingabo <mberingabo.d@husky.neu.edu>
 * @author Spencer LaChance <lachance.s@husky.neu.edu>
 */
class IncrementRower : public Rower {
    public:
        IncrementFielder* if_;
        DataFrame* df_;

        IncrementRower(DataFrame* df) {
            if_ = new IncrementFielder(df);
            df_ = df;
        }
        ~IncrementRower() { delete if_; }

        bool accept(Row& r) {
            r.visit(r.get_idx(), *if_);
        }

        DataFrame* get_new_df() {
            return if_->get_new_df();
        }

        void join_delete(Rower* other) {
            IncrementRower* o = dynamic_cast<IncrementRower*>(other);
            DataFrame* o_df = o->get_new_df();
            DataFrame* new_df_ = if_->get_new_df();
            for (int i = 0; i < o_df->nrows(); i++) {
                Row* row = new Row(new_df_->get_schema());
                o_df->fill_row(i, *row);
                new_df_->add_row(*row);
                delete row;
            }
            delete o;
        }

        Object* clone() {
            return new IncrementRower(df_);
        }
};

void pmap_example_1(DataFrame* df) {
    clock_t t;

    printf("EXAMPLE 1:\n");

    printf("PMAP:\n");
    SumRower* sr = new SumRower();

    t = clock();
    df->pmap(*sr);
    t = clock() - t;
    double time_elapsed = ((double)t)/CLOCKS_PER_SEC;
    printf("pmap() took %f seconds to execute\n", time_elapsed);

    printf("Expected df sum: 10000\n");
    printf("Actual df sum: %d\n", dynamic_cast<SumRower*>(sr)->get_total());
    delete sr;

   
    printf("MAP:\n");
    sr = new SumRower();

    t = clock();
    df->map(*sr);
    t = clock() - t;
    time_elapsed = ((double)t)/CLOCKS_PER_SEC;
    printf("map() took %f seconds to execute\n", time_elapsed);

    printf("Expected df sum: 10000\n");
    printf("Actual df sum: %d\n", dynamic_cast<SumRower*>(sr)->get_total());
    delete sr;
}

void pmap_example_2() {
    clock_t t;

    Schema* s = new Schema("IBF");
    DataFrame* df = new DataFrame(*s);
    int init_int = 0;
    bool init_bool = false;
    float init_float = 0.0;
    for (int i = 0; i < 1000000; i++) {
        Row* r = new Row(*s);
        r->set(0, init_int++);
        r->set(1, init_bool);
        r->set(2, init_float++);
        df->add_row(*r);
        init_bool = !init_bool;
        delete r;
    }
    printf("EXAMPLE 2:\n");

    printf("MAP:\n");
    Rower* sr = new IncrementRower(df);
    //printf("Original df:\n");
    // df->print();

    t = clock();
    df->map(*sr);
    t = clock() - t;
    double time_elapsed = ((double)t)/CLOCKS_PER_SEC;
    printf("map() took %f seconds to execute\n", time_elapsed);

    // DataFrame* df2 = dynamic_cast<IncrementRower*>(sr)->get_new_df();
    // printf("New df:\n");
    // printf("Every int and float should be incremented by 1 and every bool should be flipped\n");
    // df2->print();
    delete sr;
    
    // printf("PMAP:\n");
    // sr = new IncrementRower(df);
    // // printf("Original df:\n");
    // // df->print();

    // t = clock();
    // df->map(*sr);
    // t = clock() - t;
    // time_elapsed = ((double)t)/CLOCKS_PER_SEC;
    // printf("map() took %f seconds to execute\n", time_elapsed);

    // df2 = dynamic_cast<IncrementRower*>(sr)->get_new_df();
    // printf("New df:\n");
    // printf("Every int and float should be incremented by 1 and every bool should be flipped\n");
    // df2->print();

    delete sr;
    delete s;
    delete df;
    // delete df2;
}

main(int argc, char** argv) {
    ParserMain* pf = new ParserMain(argc, argv);
    DataFrame* df = new DataFrame(*(pf->get_dataframe()));
    pmap_example_1(df);
    delete df;
    return 0;
}