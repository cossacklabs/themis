package errors

// ThemisError is an operational error.
type ThemisError struct {
	msg string
}

// Error returns textual description of the error.
func (e *ThemisError) Error() string {
	return e.msg
}

// New makes an error with provided description.
func New(msg string) *ThemisError {
	return &ThemisError{msg}
}

// ThemisCallbackError is user-generated error returned from Secure Session callback.
type ThemisCallbackError struct {
	msg string
}

// Error returns textual description of the error.
func (e *ThemisCallbackError) Error() string {
	return e.msg
}

// NewCallbackError makes an error with provided description.
func NewCallbackError(msg string) *ThemisCallbackError {
	return &ThemisCallbackError{msg}
}
