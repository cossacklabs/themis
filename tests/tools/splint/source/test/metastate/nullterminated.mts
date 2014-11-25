state nullterminated
   context reference
   oneof notnullterminated, nullterminated
   annotations
      nullterminated reference ==> nullterminated
      notnullterminated reference ==> notnullterminated
   merge
      notnullterminated + * ==> notnullterminated
   transfers
      notnullterminated as nullterminated ==> error 
	"Possibly unterminated storage used as nullterminated."
   defaults
      reference notnullterminated
      parameter nullterminated
end

