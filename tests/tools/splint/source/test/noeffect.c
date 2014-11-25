typedef char *exprNode;
/*@function void exprNode_swap (sef exprNode, sef exprNode)@*/
# define exprNode_swap(e1,e2) do { exprNode m_tmp = (e1); (e1) = (e2); (e2) = m_tmp; } while (false)

