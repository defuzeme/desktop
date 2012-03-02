#!/usr/bin/env ruby

require 'socket'
require 'rubygems'
require 'json'
require 'eventmachine'

require 'defuzeme/event'

$uid = 0

module Defuzeme
  module Client
    TOKEN = 'd2KI1ONxc8123bJF'
    
    # EventMachine callback at socket initialization
    def post_init
      puts "Initialized!" if $debug
    end
    
    # EventMachine callback when receiving data
    def receive_data(data)
      # Spliting JSON packets
      data.split("\0").each do |packet|
        # Parsing an creating event
        event = Event.new JSON.parse(packet)
        puts event.inspect if $debug
        handle event
      end
    end
    
    def show_play_queue
      puts "=== Play Queue ==="
      $queue.each_with_index do |track, id|
        puts "%2d. %-20s" % [id, track["title"]]
      end
      puts "=================="
    end
    
    # EventMachine callback at socket close
    def unbind
      puts "Disconnected"
      EventMachine::stop_event_loop
    end
    
    # Handling events using dynamic method call
    def handle event
      if respond_to? event.name
        send(event.name, event)
      else
        puts "no handler for #{event.name} event!"
      end
    end
    
    # Send event to client
    def send_event event
      send_data event.to_json + "\0"
    end
    
    # Shortcut for timer callback
    def after seconds = 1
      EventMachine::Timer.new(seconds) do
        yield
      end
    end
    
    def later
      after 0.5 do
        yield
      end
    end
    
    # Defaults authentication callbacks
    def authenticationRequest event
      reply = event.new_reply 'authentication', { :token => self.class::TOKEN, :appVersion => '0.2', :deviceName => 'Ruby remote client' }
      puts "Authentication request received, sending reply: #{reply.inspect}" if $debug
      send_event reply
    end
    
    def authenticated event
      puts "Authentication successful!"
      # Default timer callback
      after 1 do
        initialized if respond_to? :initialized
      end
    end
    
    def authenticationFailed event
      puts "Authentication Failed: #{event.data["msg"]}"
      EventMachine::stop_event_loop
    end
  end

  # Main method launching EM
  def self.connect host, port, handler = Client
    EventMachine::run do
      EventMachine::connect host, port, handler
    end  
  end
end


