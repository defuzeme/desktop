#!/usr/bin/env ruby

require 'rubygems'
require 'json'

module Defuzeme
  # This class represents a network event
  class Event
    attr_accessor :name, :uid, :reply_to, :data
    
    def initialize hash, data = {}
      # Create a new event to be sent
      if hash.is_a? String
        $uid += 1
        @uid = $uid
        @name = hash
        @data = data
      # Instantiate an event from received data
      else
        @uid = hash['uid']
        @reply_to = hash['replyTo']
        @name = hash['event']
        @data = hash['data']
      end
    end
    
    # Create a reply event
    def new_reply name, data = {}
      e = Event.new name, data
      e.reply_to = self.uid
      e
    end
    
    # Create the event's hash
    def to_hash
      out = {
        'event' => name,
        'uid' => uid
      }
      out['replyTo'] = reply_to if reply_to
      out['data'] = data if data
      out
    end
    
    # Serialize the event hash into json
    def to_json
      to_hash.to_json
    end
  end
end
  
