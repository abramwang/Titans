import { TableHeaderType } from "@/app/components/tables/types";

export const headerItems: TableHeaderType[] = [
  { title: "Id", value: "id" },
  { type: "check", value: "checked" },
  { title: "Customer", value: "customer_name" },
  { title: "Email", value: "email" },
  { title: "Phone", value: "phone" },
  { title: "Joining Date", value: "date" },
  { title: "Delivery Status", value: "status" },
  { title: "Action", value: "action" }
];

export const tableData = [
  {
    id: "1",
    customer_name: "Mary Cousar",
    email: "marycousar@tailwick.com",
    date: "06 Apr, 2021",
    phone: "580-464-4694",
    status: "Active",
    value: ""
  },
  {
    id: "2",
    customer_name: "Jeff Taylor",
    email: "jefftaylor@tailwick.com",
    date: "15 Feb, 2021",
    phone: "863-577-5537",
    status: "Active"
  },
  {
    id: "3",
    customer_name: "Robert McMahon",
    email: "robertmcmahon@tailwick.com",
    date: "12 Jan, 2021",
    phone: "786-253-9927",
    status: "Active"
  },
  {
    id: "4",
    customer_name: "Michael Morris",
    email: "michaelmorris@tailwick.com",
    date: "19 May, 2021",
    phone: "805-447-8398",
    status: "Block"
  },
  {
    id: "5",
    customer_name: "Kevin Dawson",
    email: "kevindawson@tailwick.com",
    date: "14 Mar, 2021",
    phone: "213-741-4294",
    status: "Active"
  },
  {
    id: "6",
    customer_name: "Carolyn Jones",
    email: "carolynjones@tailwick.com",
    date: "07 Jun, 2021",
    phone: "414-453-5725",
    status: "Active"
  },
  {
    id: "7",
    customer_name: "Glen Matney",
    email: "glenmatney@tailwick.com",
    date: "515-395-1069",
    phone: "02 Nov, 2021",
    status: "Active"
  },
  {
    id: "8",
    customer_name: "Charles Kubik",
    email: "charleskubik@tailwick.com",
    date: "25 Sep, 2021",
    phone: "231-480-8536",
    status: "Block"
  },
  {
    id: "9",
    customer_name: "Herbert Stokes",
    email: "herbertstokes@tailwick.com",
    date: "20 Jul, 2021",
    phone: "312-944-1448",
    status: "Block"
  },
  {
    id: "10",
    customer_name: "Timothy Smith",
    email: "timothysmith@tailwick.com",
    date: "13 Dec, 2021",
    phone: "973-277-6950",
    status: "Active"
  }
];
