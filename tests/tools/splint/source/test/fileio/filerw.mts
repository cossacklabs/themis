attribute filerw
   context reference FILE *
   oneof rw_none, rw_read, rw_write, rw_either
   annotations
      read ==> rw_read
      write ==> rw_write
      rweither ==> rw_either
      rwnone ==> rw_none
   merge
      rw_read + rw_write ==> rw_none
      rw_none + * ==> rw_none
      rw_either + rw_read ==> rw_read
      rw_either + rw_write ==> rw_write
      rw_either + rw_none ==> rw_none

   transfers
      rw_read as rw_write ==> error "Must reset file between read and write."
      rw_write as rw_read ==> error "Must reset file between write and read."
      rw_none as rw_read ==> error "File in unreadable state."
      rw_none as rw_write ==> error "File in unwritable state."

      rw_either as rw_write ==> rw_write
      rw_either as rw_read ==> rw_read

end
