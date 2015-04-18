open Core.Std

(* Interface for the trie. We want to be able to input a (char*int) list and 
 * get out a string list of the best words. All internal functionality is hidden
 * from the user. *)
 
module type TRIE
sig
  
  (* Type for trie node *)
  type node
  
  (* Type for trie *)
  type trie
  
  (* The only function the user will see: finding the best words. *)
  val find_words : (char*int) list -> string list
  
end

module Word
