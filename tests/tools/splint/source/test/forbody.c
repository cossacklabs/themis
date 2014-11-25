/*@iter genericTable_elements (yield int m_el)@*/

# define genericTable_elements(m_el) \
     { for (m_ind = 0 ; m_ind < 4; m_ind++) \
       { int m_el; m_el = m_ind; m_el = 'a'; }



