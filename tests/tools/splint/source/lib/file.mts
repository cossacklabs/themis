attribute openness
   context reference FILE *
   oneof closed, open, anyopen
   annotations
      open ==> open
      closed ==> closed
      anyopen ==> anyopen
   merge
      open + closed ==> error 

   transfers
      open as closed ==> error
      closed as open ==> error

   losereference
      open ==> error "open file not closed"

   defaults 
      reference ==> open
      parameter ==> open
      result ==> open
end



