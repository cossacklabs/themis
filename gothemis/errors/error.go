package errors

type ThemisError struct {
	msg string
}

func (e *ThemisError) Error() string {
	return e.msg
}

func New(msg string) *ThemisError {
	return &ThemisError{msg}
}
