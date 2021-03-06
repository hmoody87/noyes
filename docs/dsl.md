# The Noyes DSL

Noyes uses a domain specific language to simplify the creation of arbitrarily
complicated pipelines and processing graphs.  Here are the details.

## Filter operator example.
The '>>=' operator is called the filter operator.  It modifies that data on the
left using the filter on the right.  This is similar to the way the += operator
works for numbers. Note that the >>= actually looks like a filter making it easy
to remember.  

    require 'noyes'
    data = (1..12).to_a  # An array of nonesense data.
    segmenter = Segmenter.new 4, 2 # window size, window shift
    hamming_filter = HammingWindow.new 4 # window size
    power_spec_filter = PowerSpectrumFilter.new 8 # number of ffts

    data >>= segmenter
    data >>= hamming_filter
    data >>= power_spec_filter
    data >>= dct_filter

You can expand the >>= operator out, but I think the flow is worse and there is
more repetition, particularly when you have a lot of filters in sequence.  This
is perfectly valid syntax though. Also, this is very useful if you want to keep
a reference to your original data.

    require 'noyes'
    pcm_data = (1..12).to_a
    segmenter = Segmenter.new
    hamming_filter = HammingWindow.new 4
    segmented_data = segmenter << pcm_data, 4, 2
    hamming_data = hamming_filter <<  segmented_data
    power_spectrum data = power_spec_filter hamminging_data, 8
    dct_data = dct_filter << power_spectrum_data

## Advanced filter DSLs
For most things, the filter operator is simple, easy to remember, and
very concise.  But sometimes you want to build more elaborate combinations
of filters and use them as if you had a single filter.  In this case
making a new classes for every possible combination creates an explosion
of new classes and a maintainence nightmare.  Instead, there is a simple
graph notation you can use to combine filters.  In the following example
we'll combine all the filters from a previous example and then use them
as if they were a single filter.

    serial_filter = segmenter & hamming_filter & power_spec_filter & dct_filter
    data >>= serial_filter

It's also possible to take parallel data streams and pipe them through
parallel filters as if you had only one data stream and only one filter.

    data = [stream_1,stream_2]
    parallel_filter = filter_1 | filter_2
    data >>= parallel_filter
  
It is not necessary for the data to be synchronous when using parallel filters.
When using parallel filters the number of elements going through one filter
does not have to equal the number of elements going through the second filter.

You can see that you can make arbitrarily complicated graphs of filters by
combined use of the '&' and '|' operators.  Almost identical notation is used
to specify graphs for context free grammars.  Keep in mind that '&' take
precedence over '|'.  In the example below stream 1 goes through filter 1 and
filter 2 while stream 2 goes through filters 3, 4, and 5.

    parallel_data = [stream_1,stream_2]
    big_filter = filter_1 & filter_2 | filter_3 & filter_4 & filter_5
    parallel_data >>= big_filter


