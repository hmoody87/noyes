require 'ts_fast_ruby'
require 'ts_all'
include Noyes
make_test 'Noyes', 'Incremental'
make_test 'Noyes', 'FrontEnd8k'
#make_test 'Noyes', 'RubySpeed'
make_test 'Noyes', 'IncrementalSend'
