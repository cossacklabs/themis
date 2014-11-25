attribute taintedness
   context reference char *
   oneof untainted, tainted
   annotations
      tainted reference ==> tainted
      untainted reference ==> untainted
   transfers
      tainted as untainted ==> error "Possibly tainted storage used as untainted."
      untainted as tainted ==> untainted
   merge
      tainted + untainted ==> tainted
   defaults 
      reference ==> tainted
      parameter ==> tainted
      result ==> tainted
      literal ==> untainted
      null ==> untainted 
end



