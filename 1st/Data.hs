module Data
( Data
, PC
, Register
, Memory
, check_reg   -- check_reg name          = nameに相当するregが存在するか否か。
, load_reg    -- load_reg  name regs     = nameに相当するregの値 (check_reg name = True を仮定)
, store_reg   -- store_reg name int regs = nameに相当するregの値をintに更新した新しいregs (check_reg name = True を仮定)
, print_pc    -- print_pc   pc   = pcの値を表示
, print_regs  -- print_regs regs = regsの値を表示 (32行)
, print_mem   -- print_mem address memory = memoryのaddressの位置の値を表示 (addressは4の倍数であることを仮定)
, init_pc     -- PCの初期値
, init_regs   -- regsの初期値
, init_mem    -- memoryの初期値
) where

type Data        = Maybe Int
type PC          = Data
type Register    = [Data]
type Memory      = [Data]

find :: (Eq a) => a -> [(a,b)] -> Maybe b
find a [] = Nothing
find a ((x,y):sippo) = if a==x then (Just y) else (find a sippo)

reg_names :: [(String,Int)]
reg_names =  [ ("zero",0), ("ra",1),  ("sp",2),   ("gp",3),   ("tp",4),
               ("t0",5),   ("t1",6),  ("t2",7),   ("s0/fp", 8),
               ("s1",9),   ("a0",10), ("a1",11),  ("a2",12),  ("a3",13),
               ("a4",14),  ("a5",15), ("a6",16),  ("a7",17),  ("s2",18),
               ("s3",19),  ("s4",20), ("s5",21),  ("s6",22),  ("s7",23),
               ("s8",24),  ("s9",25), ("s10",26), ("s11",27), ("t3",28),
               ("t4",29),  ("t5",30), ("t6",31),  ("s0",8),   ("fp",8) ]

check_reg :: String -> Bool
check_reg name = if name=="s0/fp"||(find name reg_names)==Nothing then False else True

load_reg :: String -> Register -> Data
load_reg name regs = temp_load regs index
  where index = find name reg_names
        temp_load regs (Just i) = regs!!i

store_reg :: String -> Data -> Register -> Register
store_reg name int regs = (take index regs)++[int]++(drop (index+1) regs)
  where index = unjust (find name reg_names)
        unjust (Just i) = i

show_data :: Data -> String
show_data (Just i) = show i
show_data Nothing  = "Undefined"

print_pc :: PC -> IO ()
print_pc pc = putStrLn $ "PC : "++(show_data pc)

print_regs :: Register -> IO [()]
print_regs regs = sequence $ map temp_print_regs (zip [0..] regs)
  where temp_print_regs (index, value) = let (name, _) = reg_names!!index in
                                         putStrLn $ name++(take (5-(length name)) (repeat ' '))++" : "++(show_data value)

print_mem :: Int -> Memory -> IO ()
print_mem address memory = let temp_address = quot address 4 in
                           if (length memory)>temp_address
                           then putStrLn $ "memory "++(show address)++" : "++(show_data (memory!!temp_address))
                           else putStrLn "Undefined"

init_pc :: PC
init_pc = Just 0

init_regs :: Register
init_regs = (Just 0):(take 31 (repeat Nothing))

init_mem :: Memory
init_mem = take 9 (repeat (Just 0))
