module Noyes
  # SpeechTrimmer trims non-speech from both ends of an audio stream. Each time
  # you enqueue audio into it you should dequeue audio out of it until dequeue
  # returns nil.  Then check for eos.  If eos is true you are done.
  # SpeechTrimmer is designed to work efficiently with live audio.
  class SpeechTrimmer
    def initialize
      @leader = 5
      @trailer = 5
      @speech_started = false
      @frame_count = 0
      @speech_started = false
      @frame_marker = BentFrameMarker.new
      @false_count=0
      @true_count=0
      @queue = []
      @eos_reached = false
      @scs = 20
      @ecs = 50
    end

    def enqueue pcm
      return if @eos_reached
      @queue << pcm
      if @frame_marker << pcm
        @false_count = 0
        @true_count += 1
      else
        @false_count += 1
        @true_count = 0
      end
      if @speech_started
        if @false_count == @ecs
          @eos_reached = true
          # only keep trailer number of frames once eos is detected.
          @queue = @queue.slice 0, @trailer
        end
      elsif @true_count > @scs
        # Discard most begining silence, keeping just a tad.
        if @leader < @queue.size
          @queue = @queue[-@leader - @scs - 1, @leader + @scs + 1]
        end
        @speech_started = true
      end
    end
    def dequeue
      if @eos_reached || (@speech_started && @queue.size > @ecs)
        @queue.shift
      end
    end
    def eos?
      @eos_reached
    end
  end    
end