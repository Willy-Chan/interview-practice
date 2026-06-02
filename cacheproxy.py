from typing import Optional

# This entire class is fully implemented for you 
# capacity
# get -> value associated with a key
# put -> put a key-value pair into the cache, respecting eviction policy (e.g.  LRU cache - oldest key you put in)

# pretend that this cache is already implemented with some eviction policy!
class Cache:
	def __init__(self, capacity) -> None: pass
	def get(self, key) -> Optional[object]: pass
	def put(self, key, value) -> None: pass

# cache proxy: between clients and remote server -> adding a microservice cache TO the remote server. Intercepts requests and caches responses.


"""
      +---------+                +-------------+
  +---| ClientA |-->> get(k) >>--|             |                                 +---------------+
  |   +---------+                |             |-->> request_from_remote(k) >>---|               |
  |       ...                    | Cache Proxy |                                 | Remote Server |
  |   +---------+                |             |--<< recv_from_remote(k, v) <<-- |               |
  +---| ClientZ |-->> get(k) >>--|             |                                 +---------------+
  |   +---------+                +-------------+
  |                                     |
  +---------<< publish(k, v) <<---------+

Requirements:
1. Every client is satisfied (always satisfy requests)
2. We want to minimize request_from_remote calls we made
3. We want to insert into local cache in the same order that the client requested it in.


Example:
1. get(foo) -> request_from_remote(foo)
2. get(bar) -> request_from_remote(bar)

Scenario A:
3. recv_from_remote(foo, foo_value)     # Doesn't matter when remote server comes in, FOO must come into the cache __before__ BAR
4. recv_from_remote(bar, bar_value)

Scenario B:
3. recv_from_remote(bar, bar_value) -> insert into heap (2, bar_value), this DOES NOT insert into the cache 
4. recv_from_remote(foo, foo_value) -> insert into heap (1, foo_value), this DOES get inserted into the cache, we start the heap popping process

rrecv_from_remote looks at the top of the heap, and only pops when request is the lowest possibly fulfilled
only increment self.request_counter when it is fulfilled
"""

# functions already implemented for you:
def publish(key, value) -> None: pass
def request_from_remote(key) -> None: pass

import heapq

# The below class is for you to implement
class CacheProxy:
    def __init__(self, cache_capacity):
        self.cache = Cache(cache_capacity)
        self.intermediate_heap = heapq.heapify([])
        self.priority_dict = {}
        self.priority = 0                               # priority of the soonest 
        self.request_counter = 1                        # request that should soonest be fulfilled


    def recv_from_remote(self, key, value) -> None:
        heapq.heappush(self.intermediate_heap, (self.priority_dict[key], key, value))

        while self.intermediate_heap[0][0] == self.request_counter:
            priority, heap_key, top_of_heap_value = heapq.heappop(self.intermediate_heap)
            self.cache.put(heap_key, top_of_heap_value)
            publish(heap_key, top_of_heap_value)
            self.request_counter += 1


        
        # raise Exception('For you to implement')

    # if I have it, just give the cached value I have. The way clients receive from their .get is by calling .publish() -> iterate through every client that wants this key, and returning the value. PUBLISH is how you service get requests.

    # if NOT cached: retrieve from source of truth: call .request_from_remote -> triggers API request at remote server. MAY take a while... but once remote has it, remote server will call recv_from_remote. 

    # assume every call to request_from_remote HAS a corresponding recv_from_remote(WHEN IT FINALLY GETS IT)
    def get(self, key) -> None:
        cache_result = self.cache.get(key)

        if cache_result is None:
            self.priority += 1
            self.priority_dict[key] = self.priority

            # not in cache, call request_from_remote
            request_from_remote(key)
        else:
            publish(key, cache_result)

        # raise Exception('For you to implement')