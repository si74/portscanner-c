package main

import (
	"fmt"
	"html"
	"log"
	"net/http"
)

var (
	port = 1
)

// Note: To test, basically tweak port above then run `go run testserver.go`

func main() {

	http.HandleFunc("/", func(w http.ResponseWriter, r *http.Request) {
		fmt.Printf("incoming request: %v\n", r.RemoteAddr)
		fmt.Fprintf(w, "Hello, %q\n", html.EscapeString(r.URL.Path))
	})

	log.Fatal(http.ListenAndServe(fmt.Sprintf(":%d", port), nil))
}
