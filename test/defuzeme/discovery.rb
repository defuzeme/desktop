#!/usr/bin/env ruby

require 'socket'
require 'rubygems'
require 'json'
require 'timeout'

module Defuzeme
  def self.retrying try = 3
    timeout = 1
    begin
      Timeout::timeout(timeout) do
        yield
      end
    rescue Timeout::Error
      if try > 0
        try -= 1
        timeout *= 2
        retry
      end
      return false
    end
    true
  end

  def self.discover
    # Open response socket
    BasicSocket.do_not_reverse_lookup = true
    recv = UDPSocket.new
    recv.bind('0.0.0.0', 3458)

    # Send request
    str = '"defuze.me:discovery"'
    data = '[]'
    success = retrying do
      sock = UDPSocket.new
      sock.setsockopt(Socket::SOL_SOCKET, Socket::SO_BROADCAST, true)
      sock.send(str, 0, '255.255.255.255', 3457)
      puts "packet sent: #{str}" if $debug
      sock.close

      # Read response
      data, addr = recv.recvfrom(999999)
      puts "packet received from %s" % addr[2] if $debug
    end
    recv.close
    JSON.parse(data)
  end
end
