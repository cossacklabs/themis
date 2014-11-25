attribute taintedness
   context reference char *
   oneof untainted, tainted
   annotations
      tainted reference ==> tainted
      untainted reference ==> untainted
      anytainted parameter ==> tainted
   transfers
      tainted as untainted ==> error "Possibly tainted storage used as untainted."
   merge
      tainted + untainted ==> tainted
   defaults 
      reference ==> tainted
      literal ==> untainted
      null ==> untainted 
end



