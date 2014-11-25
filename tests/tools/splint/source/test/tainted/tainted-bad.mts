attribute taintedness
   context reference
   oneof untainted, tainted, untainted
   defaults 
      reference ==> stainted
      parameter ==> tainted
      parameter ==> untainted
   annotations
      tainted reference ==> tainted
      untainted reference ==> untainted
      maybetainted reference ==> blue
   merge
      tainted + * ==> tainted
      tainted + junky ==> error "Splat!"
   transfers
      tainted as untainted ==> error "Possibly tainted storage used as untainted."
      tainted as tainted ==> tainted
      untainted as tainted ==> peach
      untainted as untainted ==> untainted
end



