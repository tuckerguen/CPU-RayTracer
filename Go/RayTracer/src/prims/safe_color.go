package prims

import (
	"sync"
)

//Stores 2D color data with thread safe access
type SafeColorData struct {
	Data [][]Color		//Color data
	M 	 sync.Mutex		//Mutex for concurrency handling
}

//Safely set color at index i, j
func (s *SafeColorData) Set(i, j int, c Color){
	s.M.Lock()
	s.Data[i][j] = c
	s.M.Unlock()
}