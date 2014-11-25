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
      /*untainted + tainted ==> tainted */
   defaults 
      reference ==> untainted
      parameter ==> tainted
      result ==> tainted /* Different from lib/tainted.mts */
      literal ==> untainted
      null ==> untainted 
end



