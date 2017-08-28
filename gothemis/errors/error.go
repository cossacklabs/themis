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

type ThemisCallbackError struct {
	msg string
}

func (e *ThemisCallbackError) Error() string {
	return e.msg
}

func NewCallbackError(msg string) *ThemisCallbackError {
	return &ThemisCallbackError{msg}
}
