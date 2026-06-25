use std::marker::PhantomData;
use std::mem;

use bitflags::bitflags;

use crate::ffi::xref::cref_t::*;
use crate::ffi::xref::dref_t::*;
use crate::ffi::xref::*;

use crate::idb::IDB;
use crate::Address;

pub struct XRef<'a> {
    inner: *mut xrefblk_t,
    _marker: PhantomData<&'a IDB>,
}

impl<'a> Clone for XRef<'a> {
    fn clone(&self) -> Self {
        Self {
            inner: unsafe { idalib_xref_clone(self.inner) },
            _marker: PhantomData,
        }
    }
}

impl<'a> Drop for XRef<'a> {
    fn drop(&mut self) {
        unsafe { idalib_xref_free(self.inner) }
    }
}

#[derive(Debug, Clone, Copy, PartialEq, PartialOrd, Eq, Ord, Hash)]
pub enum XRefType {
    Code(CodeRef),
    Data(DataRef),
}

#[derive(Debug, Clone, Copy, PartialEq, PartialOrd, Eq, Ord, Hash)]
#[repr(u8)]
pub enum CodeRef {
    Unknown = fl_U as _,
    FarCall = fl_CF as _,
    NearCall = fl_CN as _,
    FarJump = fl_JF as _,
    NearJump = fl_JN as _,
    Obsolete = fl_USobsolete as _,
    Flow = fl_F as _,
}

#[derive(Debug, Clone, Copy, PartialEq, PartialOrd, Eq, Ord, Hash)]
#[repr(u8)]
pub enum DataRef {
    Unknown = dr_U as _,
    Offset = dr_O as _,
    Write = dr_W as _,
    Read = dr_R as _,
    Text = dr_T as _,
    Informational = dr_I as _,
    EnumMember = dr_S as _,
}

bitflags! {
    #[derive(Debug, Clone, Copy, PartialEq, PartialOrd, Eq, Ord, Hash)]
    pub struct XRefFlags: u8 {
        const USER = XREF_USER as _;
        const TAIL = XREF_TAIL as _;
        const BASE = XREF_BASE as _;
    }
}

bitflags! {
    #[derive(Debug, Clone, Copy, PartialEq, PartialOrd, Eq, Ord, Hash)]
    pub struct XRefQuery: i32 {
        const ALL = XREF_ALL as _;
        const FAR = XREF_FAR as _;
        const DATA = XREF_DATA as _;
    }
}

impl<'a> XRef<'a> {
    pub(crate) fn from_repr(inner: *mut xrefblk_t) -> Self {
        Self {
            inner,
            _marker: PhantomData,
        }
    }

    pub fn from(&self) -> Address {
        unsafe { idalib_xref_from(self.inner).0 as _ }
    }

    pub fn to(&self) -> Address {
        unsafe { idalib_xref_to(self.inner).0 as _ }
    }

    pub fn flags(&self) -> XRefFlags {
        let flags = unsafe { idalib_xref_type(self.inner) } & !(XREF_MASK as u8);
        XRefFlags::from_bits_retain(flags)
    }

    pub fn type_(&self) -> XRefType {
        let type_ = unsafe { idalib_xref_type(self.inner) } & (XREF_MASK as u8);

        if self.is_code() {
            XRefType::Code(unsafe { mem::transmute::<u8, CodeRef>(type_) })
        } else {
            XRefType::Data(unsafe { mem::transmute::<u8, DataRef>(type_) })
        }
    }

    pub fn is_code(&self) -> bool {
        unsafe { idalib_xref_is_code(self.inner) }
    }

    pub fn is_data(&self) -> bool {
        !self.is_code()
    }

    pub fn is_user_defined(&self) -> bool {
        unsafe { idalib_xref_user(self.inner) }
    }

    pub fn next_to(&self) -> Option<Self> {
        let curr = self.clone();

        let found = unsafe { idalib_xref_next_to2(curr.inner) };

        if found {
            Some(curr)
        } else {
            None
        }
    }

    pub fn next_from(&self) -> Option<Self> {
        let curr = self.clone();

        let found = unsafe { idalib_xref_next_from2(curr.inner) };

        if found {
            Some(curr)
        } else {
            None
        }
    }
}
