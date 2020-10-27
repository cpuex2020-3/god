import Data
import Instructions

num :: String -> Bool
num []        = False
num [a]       = elem a "048"
num ('0':[b]) = elem b "048"
num (a:[b])   = elem [a,b] (map show [12,16..96])
num (a:l)     = (elem a "0123456789")&&(num l)

-- p -> PC
-- r -> Register
-- n -> code -> done
-- 4の倍数 -> メモリの中身
-- それ以外 -> quit

execute :: (PC, Register, Memory) -> IO ()
execute (pc, regs, mem) = do
  choice <- getLine
  if      choice=="p" then do print_pc pc     ; execute (pc,regs,mem)
  else if choice=="r" then do print_regs regs ; execute (pc,regs,mem)
  else if choice=="n" then do inst<-words<$>getLine ;
                              let (message:temp_inst) = (parse inst) in (do putStrLn message ; execute (if message=="Done!" then (next temp_inst (pc, regs, mem)) else (pc, regs, mem)))
  else if num choice then do print_mem (read choice) mem ; execute (pc, regs, mem)
  else putStrLn "Gott ist tot!"

main :: IO ()
main =
  execute (init_pc, init_regs, init_mem)
