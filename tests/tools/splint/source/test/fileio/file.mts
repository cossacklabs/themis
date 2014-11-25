attribute openness
   context reference FILE *
   oneof closed, open
   annotations
      open ==> open
      closed ==> closed

   merge
      open + closed ==> error

   transfers
      open as closed ==> error
      closed as open ==> error

   losereference
      open ==> error "open file not closed"

   defaults 
      reference ==> open
end



