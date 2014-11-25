/*
** A global attribute is not assosicated with a reference, but rather the
** global state of an execution.
**
** Annotations are used in pre and post conditions (requires/ensures
** clauses).
**
*/

global attribute sockets
   oneof uninitialized, initialized
   annotations
      sockets_initialized clause ==> initialized
      sockets_uninitialized clause ==> uninitialized
   preconditions
      initialized as uninitialized ==> error 
		  "Uninitialized sockets do not satisfy precondition."
   postconditions
      initialized as uninitialized ==> error 
		  "Sockets initialized, but postcondition requires uninitialized sockets."
   merge
      initialized + uninitialized ==> error 
		  "Sockets initialized on true branch, uninitialized on false branch."
      uninitialized + initialized ==> error 
		  "Sockets uninitialized on false branch, initialized on true branch."
   default
      uninitialized
end



