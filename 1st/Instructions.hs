module Instructions
( parse
, next
) where

import Data
import Data.Bits

error_message :: [String]
error_message = ["Error! Try again!"]

check_comma :: [String] -> Bool
check_comma [] = True
check_comma (s:t) = (last s)==','&&(check_comma t)

check_regs :: [String] -> Bool
check_regs [] = True
check_regs (s:t) = (check_reg s)&&(check_regs t)

check_imm :: String -> Bool
check_imm []    = False
check_imm [a]   = elem a "0123456789"
check_imm (a:b) = (check_imm [a])&&(check_imm b)

type_rrr :: String -> Bool
type_rrr opc = elem opc ["add", "sub", "xor", "or", "and"]

type_rri :: String -> Bool
type_rri opc = elem opc ["addi", "xori", "ori", "andi"]

parse :: [String] -> [String]
parse (opc:opr) | (type_rrr opc)&&(length opr)==3 =
                                 if (check_comma $ init opr)&&(check_regs $ (map init (init opr))++[(last opr)])
                                 then ["Done!", opc]++(map init (init opr))++[last opr]
                                 else error_message
                | (type_rri opc)&&(length opr)==3 = 
                                 if (check_comma $ init opr)&&(check_regs $ (map init (init opr)))&&(check_imm $ last opr)
                                 then ["Done!", opc]++(map init (init opr))++[last opr]
                                 else error_message
parse _ = error_message

find :: (Eq a) => a -> [(a,b)] -> b
find a ((x,y):sippo) = if a==x then y else (find a sippo)

op_dd :: [(String,[Data]->Data)]
op_dd = [("add",dadd), ("addi",dadd), ("sub",dsub),
         ("xor",dxor), ("xori",dxor), ("or", dor), ("ori", dor), ("and",dand), ("andi",dand)]
  where binary_data_operator f [Just i, Just j] = Just (f i j)
        binary_data_operator f _                = Nothing
        dadd = binary_data_operator (+)
        dsub = binary_data_operator (-)
        dxor = binary_data_operator xor
        dor  = binary_data_operator (.|.)
        dand = binary_data_operator (.&.)

next :: [String] -> (PC, Register, Memory) -> (PC, Register, Memory)
next (opc:opr) (pc, regs, mem) | type_rrr opc = let op_real = find opc op_dd in
                                                let ndata = op_real $ map (\s -> load_reg s regs) (tail opr) in
                                                let nregs = store_reg (head opr) ndata regs in
                                                (inc pc, nregs, mem)
                               | type_rri opc = let op_real = find opc op_dd in
                                                let ndata = op_real $ [load_reg (opr!!1) regs, Just (read (last opr))] in
                                                let nregs = store_reg (head opr) ndata regs in
                                                (inc pc, nregs, mem)
                  where inc (Just i) = Just (i+4)
