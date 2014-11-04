/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
       R A N K  A L G O R I T H M  F U N C T I O N  P R O T O T Y P E S 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int				computeRank(int M, int Q, BitSequence **matrix);
void			perform_elementary_row_operations(int flag, int i, int M, int Q, BitSequence **A);
int				find_unit_element_and_swap(int flag, int i, int M, int Q, BitSequence **A);
int				swap_rows(int i, int index, int Q, BitSequence **A);
int				determine_rank(int m, int M, int Q, BitSequence **A);
BitSequence**	create_matrix(int M, int Q);
void			display_matrix(int M, int Q, BitSequence **m);
void			def_matrix(int M, int Q, BitSequence **m,int k);
void			delete_matrix(int M, BitSequence **matrix);