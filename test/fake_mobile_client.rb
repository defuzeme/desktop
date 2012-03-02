#!/usr/bin/env ruby

# This script is intended to simulate the mobile application's network side.

require 'defuzeme'

$queue = []

module FakeClient
  include Defuzeme::Client
  TOKEN = 'd2KI1ONxc8123bJF'
  
  # Callback automatically called 1sec after authentication
  def initialized
    show_play_queue
    if $queue.size > 2
      # choose a track to move
      a = rand($queue.size - 1) + 1
      # choose where to move
      b = rand($queue.size - 1) + 1 while not b or b == a
      move_track a, b
    else
      play_test
    end
  end
  
  def move_track a, b
    puts "Now moving track #{a} to #{b}"
    move = Defuzeme::Event.new 'moveQueueElem', {
      :oldPosition => a,
      :position => b
    }
    send_event move
    later { remove_and_add_track(a, b) }
  end
  
  def remove_and_add_track a, b
    show_play_queue
    track = {'position' => b, 'type' => 'QueueTrack', 'content' => $queue[b]}
    puts "Now removing track #{track['position']} (#{track['content']['title']})"
    remove = Defuzeme::Event.new 'removeQueueElem', track
    send_event remove
    puts "Now adding track at #{a}"
    track['position'] = a;
    add = Defuzeme::Event.new 'addQueueElem', track
    send_event add
    later { show_play_queue }
    later { play_test }
  end
  
  def play_test
    print "Now trying play command: "
    next_event = Defuzeme::Event.new 'play'
    send_event next_event
    later do
      print "Now trying another play command (should be No Changes): "
      next_event = Defuzeme::Event.new 'play'
      send_event next_event
      later { pause_test }
    end
  end

  def pause_test
    print "Now trying pause command: "
    pause_event = Defuzeme::Event.new 'pause'
    send_event pause_event
    later do
      print "Now trying another pause command (should be No Changes): "
      pause_event = Defuzeme::Event.new 'pause'
      send_event pause_event
      later { stop_test }
    end
  end
  
  def stop_test
    print "Now trying stop command (should be Ok): "
    stop_event = Defuzeme::Event.new 'stop'
    send_event stop_event
    later { next_track }
  end
  
  def next_track
    print "Now sending next command (should be #{$queue.size > 1 ? 'Ok' : 'No changes'}): "
    next_event = Defuzeme::Event.new 'next'
    send_event next_event
    later { show_play_queue }
    later { test_bad_move }
  end
  
  def test_bad_move
    print "Testing bad move (should be Error): "
    send_event Defuzeme::Event.new 'moveQueueElem', {:oldPosition => $queue.size, :position => 0}
    later { test_bad_move2 }
  end

  def test_bad_move2
    print "Testing bad move (should be Error): "
    send_event Defuzeme::Event.new 'moveQueueElem', {:oldPosition => 0, :position => $queue.size}
    later { test_dumb_move }
  end

  def test_dumb_move
    print "Testing dumb move (should be noChanges): "
    send_event Defuzeme::Event.new 'moveQueueElem', {:oldPosition => 0, :position => 0}
    later { EventMachine::stop_event_loop }
  end
  
  # Event callbacks
  def newQueueElem event
    $queue.insert(event.data['position'], event.data["content"])
  end
  
  def removeQueueElem event
    $queue.delete_at(event.data['position'])
  end
  
  def popQueue
    $queue.shift
  end
  
  def ok event
    puts "Ok"
  end

  def error event
    puts "Error"
  end
  
  def noChanges event
    puts "No changes"
  end
  
  def play event
    puts "Client is currently playing"
  end
  
  def pause event
    puts "Client is currently paused"
  end
end

Defuzeme.connect 'localhost', 3456, FakeClient
