module Codegen where

import Control.Monad
import Control.Monad.Free


--------------------------------------------------------------------------------
type ID = Integer



--------------------------------------------------------------------------------
data ValueF
  = FixedValue
  | UnitValue
  | AbstractValue ID
  deriving (Eq, Ord, Show)



--------------------------------------------------------------------------------
data Word32F
  = FixedWord32 Integer
  | AbstractWord32 ID
  deriving (Eq, Ord, Show)

instance Num Word32F where
  fromInteger n = FixedWord32 n
  (+)           = undefined
  (-)           = undefined
  (*)           = undefined
  abs           = undefined
  signum        = undefined

data Word64F
  = FixedWord64 Integer
  | AbstractWord64 ID
  deriving (Eq, Ord, Show)

instance Num Word64F where
  fromInteger n = FixedWord64 n
  (+)           = undefined
  (-)           = undefined
  (*)           = undefined
  abs           = undefined
  signum        = undefined

data Int32F
  = FixedInt32 Integer
  | AbstractInt32 ID
  deriving (Eq, Ord, Show)

instance Num Int32F where
  fromInteger n = FixedInt32 n
  (+)           = undefined
  (-)           = undefined
  (*)           = undefined
  abs           = undefined
  signum        = undefined

data Int64F
  = FixedInt64 Integer
  | AbstractInt64 ID
  deriving (Eq, Ord, Show)

instance Num Int64F where
  fromInteger n = FixedInt64 n
  (+)           = undefined
  (-)           = undefined
  (*)           = undefined
  abs           = undefined
  signum        = undefined

--------------------------------------------------------------------------------
data InterpF x
  = StkPeek Word32F (ValueF -> x)
  | StkPush ValueF x
  | StkPopN Word32F x
  | StkSetN Word32F ValueF x
  | SetAccu ValueF x
  | GetAccu (ValueF -> x)
  | SetEnv ValueF x
  | GetEnv (ValueF -> x)
  | SetExtraArgs Word64F x
  | GetExtraArgs (Word64F -> x)
  | SetPC Word64F x
  | GetPC (Word64F -> x)
  | MkInt32 Int32F (ValueF -> x)
  | MkInt64 Int64F (ValueF -> x)
  | PCInt32 (Int32F -> x)
  | PCWord32 (Word32F -> x)
  | GetField ValueF Word32F (ValueF -> x)
  | SetField ValueF Word32F ValueF x
  | SetCode ValueF Word64F x
  | GetCode ValueF (Word64F -> x)

instance Functor InterpF where
  fmap f (StkPeek n x)      = StkPeek n (f . x)
  fmap f (StkPush v x)      = StkPush v (f x)
  fmap f (StkPopN v x)      = StkPopN v (f x)
  fmap f (StkSetN n v x)    = StkSetN n v (f x)

  fmap f (SetAccu v x)      = SetAccu v (f x)
  fmap f (GetAccu x)        = GetAccu (f . x)
  fmap f (SetEnv v x)       = SetEnv v (f x)
  fmap f (GetEnv x)         = GetEnv (f . x)
  fmap f (SetExtraArgs v x) = SetExtraArgs v (f x)
  fmap f (GetExtraArgs x)   = GetExtraArgs (f . x)
  fmap f (SetPC v x)        = SetPC v (f x)
  fmap f (GetPC x)          = GetPC (f . x)

  fmap f (MkInt32 v x)      = MkInt32 v (f . x)
  fmap f (MkInt64 v x)      = MkInt64 v (f . x)

  fmap f (PCInt32 x)        = PCInt32 (f . x)
  fmap f (PCWord32 x)       = PCWord32 (f . x)

  fmap f (GetField v i x)   = GetField v i (f . x)
  fmap f (SetField v i n x) = SetField v i n (f x)

  fmap f (GetCode v x)      = GetCode v (f . x)
  fmap f (SetCode v n x)    = SetCode v n (f x)

type Interp = Free InterpF



--------------------------------------------------------------------------------
stkPeek :: Word32F -> Interp ValueF
stkPeek n = liftF $ StkPeek n id

stkPush :: ValueF -> Interp ()
stkPush v = liftF $ StkPush v ()

stkPopN :: Word32F -> Interp ()
stkPopN n = liftF $ StkPopN n ()

stkSetN :: Word32F -> ValueF -> Interp ()
stkSetN n v = liftF $ StkSetN n v ()



--------------------------------------------------------------------------------
setAccu :: ValueF -> Interp ()
setAccu v = liftF $ SetAccu v ()

getAccu :: Interp ValueF
getAccu = liftF $ GetAccu id

setEnv :: ValueF -> Interp ()
setEnv v = liftF $ SetEnv v ()

getEnv :: Interp ValueF
getEnv = liftF $ GetEnv id

setExtraArgs :: Word64F -> Interp ()
setExtraArgs v = liftF $ SetExtraArgs v ()

getExtraArgs :: Interp Word64F
getExtraArgs = liftF $ GetExtraArgs id

setPC :: Word64F -> Interp ()
setPC v = liftF $ SetPC v ()

getPC :: Interp Word64F
getPC = liftF $ GetPC id



--------------------------------------------------------------------------------
mkInt32 :: Int32F -> Interp ValueF
mkInt32 w = liftF $ MkInt32 w id

mkInt64 :: Int64F -> Interp ValueF
mkInt64 w = liftF $ MkInt64 w id



--------------------------------------------------------------------------------
pcInt32 :: Interp Int32F
pcInt32 = liftF $ PCInt32 id

pcWord32 :: Interp Word32F
pcWord32 = liftF $ PCWord32 id



--------------------------------------------------------------------------------
getField :: ValueF -> Word32F -> Interp ValueF
getField v i = liftF $ GetField v i id

setField :: ValueF -> Word32F -> ValueF -> Interp ()
setField v i n = liftF $ SetField v i n ()

getCode :: ValueF -> Interp Word64F
getCode v = liftF $ GetCode v id

setCode :: ValueF -> Word64F -> Interp ()
setCode v n = liftF $ SetCode v n ()
