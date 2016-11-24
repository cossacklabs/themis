package errors

type themisError struct{
    msg string
}

func (e *themisError) Error() string {
 return e.msg
}

func New(msg string) *themisError{
    return &themisError{msg}
}