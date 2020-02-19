//lang::CwC

#include "modified_dataframe.h"
#include "parser_main.h"
#include "helper.h"

/**
 * A Fielder that adds up every int it finds in a row.
 * 
 * @author David Mberingabo <mberingabo.d@husky.neu.edu>
 * @author Spencer LaChance <lachance.s@husky.neu.edu>
 */
class SumFielder : public Fielder {
    public:
        size_t total_;

        void start(size_t r) { total_ = 0; }
        void done() { }

        SumFielder(size_t total) {
            total_ = total;
        }
        ~SumFielder() { }

        void accept(bool b) { }
        void accept(float f) { }
        void accept(String* s) { }
        void accept(int i) {
            total_ += i;
        }

        size_t get_total() {
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
        size_t total_;

        SumRower() {
            total_ = 0;
            sf_ = new SumFielder(total_);
        }
        ~SumRower() { delete sf_; }

        bool accept(Row& r) {
            r.visit(r.get_idx(), *sf_);
            total_ += sf_->get_total();
        }

        size_t get_total() {
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
 * The new DataFrame also contains a new column with the sums of all of the ints in the DF up to
 * that row.
 * 
 * @author David Mberingabo <mberingabo.d@husky.neu.edu>
 * @author Spencer LaChance <lachance.s@husky.neu.edu>
 */
class IncrementFielder : public Fielder {
    public:
        // Original DF
        DataFrame* df_;
        // New DF
        DataFrame* new_df_;
        // Row that will be added to the new DF after every visit() call.
        Row* new_row_;
        // The index of the row we're traversing within the DF.
        size_t row_index_;
        // The index within the row we're traversing.
        size_t col_index_;
        // Rower used to calculate the sums for the new column.
        SumRower* sr_;
        // Name of the new sum column.
        String* new_col_name_;

        IncrementFielder(DataFrame* df) {
            df_ = df;
            Schema* new_schema = new Schema(df_->get_schema());
            new_col_name_ = new String("sums");
            new_df_ = new DataFrame(*new_schema);
            new_df_->add_column(new FloatColumn(), new_col_name_);
            new_row_ = new Row(new_df_->get_schema());
            row_index_ = 0;
        }
        ~IncrementFielder() {
            delete new_df_;
            delete new_row_;
            delete new_col_name_;
         }

        void start(size_t r) {
            col_index_ = 0;
            sr_ = new SumRower();
        }
        void done() {
            // Temporarily setting the field in the sum column to 0. Will be
            // replaced after map is ran.
            new_row_->set(col_index_, 0.0f);
            new_df_->add_row(*new_row_);
            // Calculate the sum of all the ints up to the current row and add
            // it to the new column in the new DF.
            new_df_->map(*sr_);
            new_df_->set(new_df_->ncols() - 1, row_index_, (float) sr_->get_total());
            row_index_++;
            delete sr_;
        }

        void accept(bool b) { 
            new_row_->set(col_index_, !b);
            col_index_++;
        }
        void accept(float f) {
            new_row_->set(col_index_, f + 1);
            col_index_++;
        }
        void accept(String* s) { 
            new_row_->set(col_index_, s->clone());
            col_index_++;
        }
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
 * it finds in a row using a fielder. It then adds these new values to a new DataFrame.
 * The new DataFrame also contains a new column with the sums of all of the ints in the DF up to
 * that row.
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
            
            int ncols = new_df_->ncols();
            int nrows = new_df_->nrows();
            // The last field in this Rower's DF's sum column.
            // Will be added to every field in other's DF's sum column.
            float r1_sum = new_df_->get_float(ncols - 1, nrows - 1);
            float r2_sum;
            Row* row = new Row(new_df_->get_schema());
            for (int i = 0; i < o_df->nrows(); i++) {
                r2_sum = o_df->get_float(ncols - 1, i);
                o_df->set(ncols - 1, i, r1_sum + r2_sum);
                o_df->fill_row(i, *row);
                new_df_->add_row(*row);
            }
            delete row;
            delete o;
        }

        Object* clone() {
            return new IncrementRower(df_);
        }
};

void map_example_1(DataFrame* df) {
    printf("EXAMPLE 1 MAP:\n");
    SumRower* sr = new SumRower();
    df->map(*sr);
    delete sr;
}

void pmap_example_1(DataFrame* df) {
    printf("EXAMPLE 1 PMAP:\n");
    SumRower* sr = new SumRower();
    df->pmap(*sr);
    delete sr;
}

void map_example_2(DataFrame* df) {
    printf("EXAMPLE 2 MAP:\n");
    IncrementRower* ir = new IncrementRower(df);
    df->map(*ir);
    // ir->get_new_df()->print();
    delete ir;
}

void pmap_example_2(DataFrame* df) {
    printf("EXAMPLE 2 PMAP:\n");
    IncrementRower* ir = new IncrementRower(df);
    df->pmap(*ir);
    // ir->get_new_df()->print();
    delete ir;
}

int main(int argc, char** argv) {
    ParserMain* pf = new ParserMain(argc, argv);
    DataFrame* df = new DataFrame(*(pf->get_dataframe()));
    Sys sys;
    if (strcmp(argv[1], "-p") == 0) {
        sys.exit_if_not(strcmp(argv[2], "-e") == 0, "Please specify which example you would like to run using -e [1,2]");
        if (strcmp(argv[3], "1") == 0) {
            pmap_example_1(df);
        } else if (strcmp(argv[3], "2") == 0) {
            pmap_example_2(df);
        } else {
            sys.exit_if_not(false, "Please specify which example you would like to run using -e [1,2]");
        }
    } else {
        sys.exit_if_not(strcmp(argv[1], "-e") == 0, "Please specify which example you would like to run using -e [1,2]");
        if (strcmp(argv[2], "1") == 0) {
            map_example_1(df);
        } else if (strcmp(argv[2], "2") == 0) {
            map_example_2(df);
        } else {
            sys.exit_if_not(false, "Please specify which example you would like to run using -e [1,2]");
        }
    }
    delete df;
    return 0;
}