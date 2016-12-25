{-# LANGUAGE LambdaCase #-}
module Main where

import Codegen
import Control.Applicative
import Control.Monad
import System.Environment (getArgs)



insts =
  -- ACCn
  [ stkPeek 0 >>= setAccu
  , stkPeek 1 >>= setAccu
  , stkPeek 2 >>= setAccu
  , stkPeek 3 >>= setAccu
  , stkPeek 4 >>= setAccu
  , stkPeek 5 >>= setAccu
  , stkPeek 6 >>= setAccu
  , stkPeek 7 >>= setAccu
  , do
    n <- pcWord32
    stkPeek n >>= setAccu
  -- PUSH
  , do
    a <- getAccu
    stkPush a
  -- PUSHACCn
  , getAccu >>= stkPush >> stkPeek 0 >>= setAccu
  , getAccu >>= stkPush >> stkPeek 1 >>= setAccu
  , getAccu >>= stkPush >> stkPeek 2 >>= setAccu
  , getAccu >>= stkPush >> stkPeek 3 >>= setAccu
  , getAccu >>= stkPush >> stkPeek 4 >>= setAccu
  , getAccu >>= stkPush >> stkPeek 5 >>= setAccu
  , getAccu >>= stkPush >> stkPeek 6 >>= setAccu
  , getAccu >>= stkPush >> stkPeek 7 >>= setAccu
  , do
    n <- pcWord32
    v <- getAccu
    a <- stkPeek n
    setAccu a
  -- POP
  , do
    n <- pcWord32
    stkPopN n
  -- ASSIGN
  , do
    n <- pcWord32
    a <- getAccu
    stkSetN n a
    setAccu UnitValue
  -- ENVACC
  , getEnv >>= \env -> getField env 1 >>= setAccu
  , getEnv >>= \env -> getField env 2 >>= setAccu
  , getEnv >>= \env -> getField env 3 >>= setAccu
  , getEnv >>= \env -> getField env 4 >>= setAccu
  , do
    n <- pcWord32
    env <- getEnv
    v <- getField env n
    setAccu v
  -- PUSHENVACC
  , getAccu >>= stkPush >> getEnv >>= \env -> getField env 1 >>= setAccu
  , getAccu >>= stkPush >> getEnv >>= \env -> getField env 2 >>= setAccu
  , getAccu >>= stkPush >> getEnv >>= \env -> getField env 3 >>= setAccu
  , getAccu >>= stkPush >> getEnv >>= \env -> getField env 4 >>= setAccu
  , do
    getAccu >>= stkPush
    n <- pcWord32
    env <- getEnv
    v <- getField env n
    setAccu v
  -- PUSH_RETADDR
  , return ()
  -- APPLY
  , return ()
  , return ()
  , return ()
  , return ()
  -- APPTERM
  , return ()
  , do
      n <- pcWord32
      arg1 <- stkPeek 0
      stkPopN n
      stkPush arg1
      a <- getAccu
      pc <- getCode a
      setPC pc
      setEnv a
  , do
      n <- pcWord32
      arg2 <- stkPeek 0
      arg1 <- stkPeek 1
      stkPopN n
      stkPush arg1
      stkPush arg2
      a <- getAccu
      pc <- getCode a
      setPC pc
      setEnv a
      extraArgs <- getExtraArgs
      setExtraArgs (extraArgs + 1)
  , do
      n <- pcWord32
      arg3 <- stkPeek 0
      arg2 <- stkPeek 1
      arg1 <- stkPeek 2
      stkPopN n
      stkPush arg1
      stkPush arg2
      stkPush arg3
      a <- getAccu
      pc <- getCode a
      setPC pc
      setEnv a
      extraArgs <- getExtraArgs
      setExtraArgs (extraArgs + 2)
  -- RETURN
  , return ()
  -- RESTART
  , return ()
  -- GRAB
  , return ()
  -- CLOSURE
  , return ()
  -- CLOSUREREC
  , return ()
  -- OFFSECLOSUREM2
  , return ()
  -- OFFSETCLOSURE
  , return ()
  , return ()
  , return ()
  -- PUSHOFFSECLOSUREM2
  , return ()
  -- PUSHOFFSETCLOSURE
  , return ()
  , return ()
  , return ()
  -- GETGLOBAL
  , return ()
  -- PUSHGETGLOBAL
  , return ()
  -- GETGLOBALFIELD
  , return ()
  -- PUSHGETGLOBALFIELD
  , return ()
  -- SETGLOBAL
  , return ()
  -- ATOM
  , return ()
  , return ()
  -- PUSHATOM
  , return ()
  , return ()
  -- MAKEBLOCK
  , return ()
  , return ()
  , return ()
  , return ()
  -- MAKEFLOATBLOCK
  , return ()
  -- GETFIELD
  , return ()
  , return ()
  , return ()
  , return ()
  , return ()
  -- GETFLOATFIELD
  , return ()
  -- SETFIELD
  , return ()
  , return ()
  , return ()
  , return ()
  , return ()
  -- SETFLOATFIELD
  , return ()
  -- VECTLENGTH
  , return ()
  -- GETVECTITEM
  , return ()
  -- SETVECTITEM
  , return ()
  -- GETSTRINGCHAR
  , return ()
  -- SETSTRINGCHAR
  , return ()
  -- BRANCH
  , return ()
  -- BRANCHIF
  , return ()
  -- BRANCHIFNOT
  , return ()
  -- SWITCH
  , return ()
  -- BOOLNOT
  , return ()
  -- PUSHTRAP
  , return ()
  -- POPTRAP
  , return ()
  -- RAISE
  , return ()
  -- CHECK_SIGNALS
  , return ()
  -- CCALL
  , return ()
  , return ()
  , return ()
  , return ()
  , return ()
  , return ()
  -- CONST
  , mkInt64 0 >>= setAccu
  , mkInt64 1 >>= setAccu
  , mkInt64 2 >>= setAccu
  , mkInt64 3 >>= setAccu
  , pcInt32 >>= mkInt32 >>= setAccu
  -- PUSHCONST
  , getAccu >>= stkPush >> mkInt64 0 >>= setAccu
  , getAccu >>= stkPush >> mkInt64 1 >>= setAccu
  , getAccu >>= stkPush >> mkInt64 2 >>= setAccu
  , getAccu >>= stkPush >> mkInt64 3 >>= setAccu
  , getAccu >>= stkPush >> pcInt32 >>= mkInt32 >>= setAccu
  -- NEGINT
  , return ()
  -- ADDINT
  , return ()
  -- SUBINT
  , return ()
  -- MULINT
  , return ()
  -- DIVINT
  , return ()
  -- MODINT
  , return ()
  -- ANDINT
  , return ()
  -- ORINT
  , return ()
  -- XORINT
  , return ()
  -- LSLINT
  , return ()
  -- LSRINT
  , return ()
  -- ASRINT
  , return ()
  -- EQ
  , return ()
  -- NEQ
  , return ()
  -- LTINT
  , return ()
  -- LEINT
  , return ()
  -- GTINT
  , return ()
  -- GEINT
  , return ()
  -- OFFSETINT
  , return ()
  -- OFFSETREF
  , return ()
  -- ISINT
  , return ()
  -- GETMETHOD
  , return ()
  -- BEQ
  , return ()
  -- BNEQ
  , return ()
  -- BLTINT
  , return ()
  -- BLEINT
  , return ()
  -- BGTINT
  , return ()
  -- BGEINT
  , return ()
  -- ULTINT
  , return ()
  -- UGEINT
  , return ()
  -- BULTINT
  , return ()
  -- BUGEINT
  , return ()
  -- GETPUBMET
  , return ()
  -- GETDYNMET
  , return ()
  -- STOP
  , return ()
  -- EVENT
  , return ()
  -- BREAK
  , return ()
  ]



main :: IO ()
main = getArgs >>= \case
  ["x86", out] ->
    writeFile out ""
  _ ->
    putStrLn "Usage: ./Interpreter.hs [arch] [out-path]"
