# Python modules
import time
import mmap
import os
import sys
import hashlib

# 3rd party modules
import posix_ipc

# Utils for this demo
import utils

utils.say("Oooo 'ello, I'm Mrs. Conclusion!")

params = utils.read_params()

# The semaphore and shared memory has already been created
# I just need their names to get handles to them.
memory = posix_ipc.SharedMemory(params["SHARED_MEMORY_NAME"])
semaphoreSim = posix_ipc.Semaphore(params["SEMAPHORE_SIM"]) + memory
semaphoreCog = posix_ipc.Semaphore(params["SEMAPHORE_COG"]) + memory

# MMap the shared memory
mapfile = mmap.mmap(memory.fd, memory.size)

# Once I've mmapped the file descriptor, I can close it without 
# interfering with the mmap. This also demonstrates that os.close() is a
# perfectly legitimate alternative to the SharedMemory's close_fd() method.
os.close(memory.fd)

what_i_wrote = ""

for i in range(0, params["ITERATIONS"]):
    utils.say("iteration %d" % i)
    
    # Wait for the other process to free up the semaphore.
    utils.say("Waiting to acquire the semaphore")
    semaphoreCog.acquire()

    # Reading
    what_i_read = utils.read_from_memory(mapfile)
    utils.say("Finally sth new : " + what_i_read)

    # write in the shared memory
    what_i_wrote = "Writin sth at iteration "  + str(i)
    utils.write_to_memory(mapfile, what_i_wrote)
    
    #if not params["LIVE_DANGEROUSLY"]:
    utils.say("Releasing the semaphore")
    semaphoreSim.release()

semaphore.close()
mapfile.close()

utils.say("")
utils.say("%d iterations complete" % (i + 1))
